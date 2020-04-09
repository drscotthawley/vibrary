/*
  ==============================================================================

   Copyright 2020 Art+Logic, Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include <memory>
#include <vector>

#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/attr_value_util.h"

#include "GraphReader.h"
#include "NetFeeder.h"
#include "Settings.h"
#include "Spectrogram.h"

namespace tf  = tensorflow;
namespace tfo = tf::ops;
using Shape   = std::vector<tf::int64>;
using Tensors = std::vector<tf::Tensor>;
using tf::Status;

// Set to non-zero to save the spectrograms fed to the network as PNGs
#define qSaveSpectrogramInput 0

namespace
{
   enum class TensorDim
   {
      Batch, Row, Col, Channel
   };

   /**
    * Given an image file name, read in the data, try to decode it as an image,
    * resize it to the requested size, and then scale the values as desired.
    */
   Status ReadTensorFromImageFile(const std::string& file_name,
                                  int64_t input_width, int64_t input_height,
                                  Tensors* out_tensors);

   /**
    * Copy channel data from input to tensor.
    * If input has smaller dimension than tensor, pad with zeros.
    * If input has larger dimension than tensor, crop.
    */
   void CopySpectrogramTo(tf::Tensor& tensor, TensorDim rowDim, TensorDim colDim,
                          const ASpectrogram& input, int channel, bool flipY = true);

#if qSaveSpectrogramInput

   /**
    * Look at contents of tensor as a spectrogram.
    * \param channel which channel to include in the single channel output.
    */
   ASpectrogram CopySpectrogramFrom(const tf::Tensor& tensor, int channel);

#endif
}


ANetFeeder::ANetFeeder(const std::string& graphFilename,
                       const std::string& inputLayerName,
                       const std::string& outputLayerName,
                       const std::vector<std::string>& labels)
: fInputLayerName(inputLayerName)
, fOutputLayerName(outputLayerName)
, fLabels(labels)
{
   AGraphReader::ReadModel(graphFilename, fSavedModel);

   // !!! TEMP - try to fix the old model files with the wrong axis in batch normal layers
  // this->ModBatchNormNodes();

   fGraph = fSavedModel.meta_graph_def.graph_def();
}

void ANetFeeder::ModBatchNormNodes()
{
   using namespace std;

   auto graph = fSavedModel.meta_graph_def.mutable_graph_def();
   const auto count = graph->node_size();
   tf::NodeDef node;
   for (int n = 0; n < count; ++n)
   {
      constexpr const char* const kName = "batch_normalization_1";

      auto node = graph->mutable_node(n);
      if (node->name().substr(0, strlen(kName)) == kName)
      {
         auto attr = node->mutable_attr();

#if 0
         transform(attr->begin(), attr->end(), ostream_iterator<string>(cout),
          [](const auto& keyValPair)
          {
             return keyValPair.first + '\t' + tf::SummarizeAttrValue(keyValPair.second) + '\n';
          });
#endif
         
         auto attrValIt = attr->find("axis");
         if (attrValIt != attr->end())
         {
            std::cout << n << ':' << node->name() << ':' << node->op() << '\n';

            tf::SetAttrValue(-1, &((*attr)["axis"]));

            std::cout << "axis <- " << tf::SummarizeAttrValue((*attr)["axis"]) << '\n';
         }

         attrValIt = attr->find("data_format");
         if (attrValIt != attr->end())
         {
            std::cout << n << ':' << node->name() << ':' << node->op() << '\n';

            std::cout << "data_format -> "
             << tf::SummarizeAttrValue(fSavedModel.meta_graph_def.graph_def()
                                       .node(n).attr().at("data_format"))
             << '\n';

            transform(attr->begin(), attr->end(), ostream_iterator<string>(cout),
             [](const auto& keyValPair)
             {
                return keyValPair.first + '\t' + tf::SummarizeAttrValue(keyValPair.second) + '\n';
             });

            tf::SetAttrValue("NHWC", &((*attr)["data_format"]));
            
            std::cout << "data_format <- "
             << tf::SummarizeAttrValue(fSavedModel.meta_graph_def.graph_def()
                                       .node(n).attr().at("data_format"))
             << '\n';
         }
      }
   }
}


std::pair<int64_t, int64_t> ANetFeeder::GraphInputsShape()
{
   auto rows = int64_t(0L);
   auto columns = int64_t(0L);
   const auto count = fGraph.node_size();
   tf::NodeDef node;
   for (int n = 0; n < count; ++n)
   {
      auto node = fGraph.node(n);
      if (fInputLayerName == node.name())
      {
         auto shape = node.attr().find("shape")->second.shape();
         rows = shape.dim(1).size();
         columns = shape.dim(2).size();
         break;
      }
   }
   return { rows, columns };
}


Status ANetFeeder::FeedForward(const ASpectrogram& input)
{
#if qSaveSpectrogramInput // DEBUG
   if (ASettings::Instance().SaveSpectrogramPng())
   {
      input.SaveAsPng(ASettings::Instance().DebugRootPath());
   }
#endif

   int64_t tensorRows, tensorCols;
   std::tie(tensorRows, tensorCols) = this->GraphInputsShape();

   auto tensor = tf::Tensor(tf::DT_FLOAT, { 1L, tensorRows, tensorCols, 1L });
   CopySpectrogramTo(tensor, TensorDim::Row, TensorDim::Col, input, 0);

   return this->FeedForward(tensor);
}


Status ANetFeeder::FeedForward(const std::string& imageFilename)
{
   int64_t input_width, input_height;
   std::tie(input_height, input_width) = this->GraphInputsShape();
   
   Tensors image_tensors;
   auto status = ReadTensorFromImageFile(imageFilename, input_width, input_height,
                                         &image_tensors);
   if (!status.ok())
   {
      LOG(ERROR) << "Failed to read input image: " << status;
      return status;
   }

   LOG(INFO) << image_tensors[1].DebugString();
   return this->FeedForward(image_tensors[0]);
}


Status ANetFeeder::FeedForward(const tf::Tensor& input_tensor)
{
   fOutputs.clear();
   
#if qSaveSpectrogramInput // DEBUG: Get contents of tensor as PNG.

   if (ASettings::Instance().SaveTensorPng())
   {
      CopySpectrogramFrom(input_tensor, 0).SaveAsPng(ASettings::Instance().DebugRootPath());
   }
   
#endif

   return fSavedModel.session->Run({{ fInputLayerName, input_tensor }},
                                   { fOutputLayerName }, {}, &fOutputs);
}


Status ANetFeeder::LabeledTopK(int k, std::vector<std::string>& labels) const
{
   std::vector<int> indices;
   auto status = this->TopK(k, indices);
   if (status.ok())
   {
      labels.reserve(k);
      transform(begin(indices), end(indices), begin(labels),
       [this](int i)
       {
         return fLabels.at(i);
       });
   }
   return status;
}


Status ANetFeeder::TopK(int k, std::vector<int>& classes) const
{
   auto root = tf::Scope::NewRootScope();
   
   std::string output_name = "top_k";
   tfo::TopK(root.WithOpName(output_name), fOutputs[0], k);
   tf::GraphDef graph;
   TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));
   
   std::unique_ptr<tf::Session> session(tf::NewSession(tf::SessionOptions()));
   TF_RETURN_IF_ERROR(session->Create(graph));
   // The TopK node returns two outputs, the scores and their original indices,
   // so we have to append :0 and :1 to specify them both.
   Tensors out_tensors;
   TF_RETURN_IF_ERROR(session->Run({}, {output_name + ":0", output_name + ":1"},
                                   {}, &out_tensors));
   //!!!tf::Tensor& scores = out_tensors[0];
   tf::Tensor& indices = out_tensors[1];

  //!!! auto scores_flat = scores.flat<float>();
   auto indices_flat = indices.flat<tf::int32>();
   
   classes.reserve(k);
   for (int pos = 0; pos < k; ++pos)
   {
      const int classId = indices_flat(pos);
      //!!!const float score = scores_flat(pos);
      classes.push_back(classId);
   }
   return Status::OK();
}



namespace
{
   void CopySpectrogramTo(tf::Tensor& tensor, TensorDim rowDim, TensorDim colDim,
                        const ASpectrogram& input, int channel, bool flipY/*=true*/)
   {
      assert(tensor.dtype() == tf::DT_FLOAT && "We don't support copying non-float tensors");
      
      // This is based on https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/framework/tensor_util.cc#L35
      
      // We use StringPiece as a convenient map over the tensor buffer,
      // but we cast the type to get to the underlying buffer to do the
      // copy.
      tf::StringPiece tensorData = tensor.tensor_data();
      char* dst = const_cast<char*>(tensorData.data());
      
      // Pad with zeros
      std::memset(dst, 0, tensor.NumElements() * sizeof(float));
      
      auto src = reinterpret_cast<const char*>(input.Channel(channel).data());
      size_t inputCols = input.Shape().second;

      int64_t tensorRows = tensor.shape().dim_size((int)rowDim);
      int64_t tensorCols = tensor.shape().dim_size((int)colDim);
      
      size_t srcOff = inputCols * sizeof(float);
      size_t dstOff = tensorCols * sizeof(float);
      size_t rowBytes = std::min(srcOff, dstOff);
      
      size_t rows = std::min((size_t)tensorRows, input.Shape().first);
      const char* const dstEnd = dst + rows * dstOff;
      
      if (flipY)
      {
         src = src + (rows - 1) * srcOff;
         srcOff = -srcOff;
      }
      
      for ( ; dstEnd != dst; dst += dstOff, src += srcOff)
      {
         std::memcpy(dst, src, rowBytes);
      }
   }
   

#if qSaveSpectrogram // DEBUG

   ASpectrogram CopySpectrogramFrom(const tf::Tensor& tensor, int channel)
   {
      assert(tensor.dtype() == tf::DT_FLOAT && "We don't support copying non-float tensors");
      assert(tensor.shape().dims() == 4 && "We assume Tensor style [batch, row, col, channel] indexing.");
      
      // We use StringPiece as a convenient map over the tensor buffer,
      // but we cast the type to get to the underlying buffer to do the
      // copy.
      tf::StringPiece tensorData = tensor.tensor_data();
      auto src = reinterpret_cast<const float*>(tensorData.data());
      src += channel;

      int64_t rows = tensor.shape().dim_size((int)TensorDim::Row);
      int64_t cols = tensor.shape().dim_size((int)TensorDim::Col);
      int64_t channels = tensor.shape().dim_size((int)TensorDim::Channel);
      
      ASpectrogram::Spectrum outChannel(rows * cols);
      float* dst = outChannel.data();
      
      if (1 == channels)
      {
         memcpy(dst, src, rows * cols * sizeof(float));
      }
      else
      {
         const float* const dstEnd = dst + rows * cols;
         while (dstEnd != dst)
         {
            *dst++ = *src;
            src += channels;
         }
      }
      
      return ASpectrogram({ outChannel }, rows, cols, "SpectroFromTensor");
   }

#endif
 
   
   Status ReadTensorFromImageFile(const std::string& file_name,
                                  int64_t input_width, int64_t input_height,
                                  Tensors* out_tensors)
   {
      auto root = tf::Scope::NewRootScope();
      using namespace ::tf::ops;  // NOLINT(build/namespaces)
      
      std::string input_name = "file_reader";
      std::string original_name = "identity";
      std::string output_name = "cropped";
      auto file_reader =
       tfo::ReadFile(root.WithOpName(input_name), file_name);
      
      // Now try to figure out what kind of file it is and decode it.
      const int wanted_channels = 1;
      tf::Output image_reader;
      if (absl::EndsWith(file_name, ".png"))
      {
         image_reader = DecodePng(root.WithOpName("png_reader"), file_reader,
                                  DecodePng::Channels(wanted_channels));
      }
      else if (absl::EndsWith(file_name, ".gif"))
      {
         image_reader = DecodeGif(root.WithOpName("gif_reader"), file_reader);
      }
      else
      {
         // There is a call to decode and crop
         // Assume if it's neither a PNG nor a GIF then it must be a JPEG.
         image_reader = DecodeJpeg(root.WithOpName("jpeg_reader"), file_reader,
                                   DecodeJpeg::Channels(wanted_channels));
      }
      
      // Also return identity so that we can know the original dimensions and
      // optionally save the image out with bounding boxes overlaid.
      auto original_image = Identity(root.WithOpName(original_name), image_reader);
      
      // Now cast the image data to float so we can do normal math on it.
      auto float_caster = Cast(root.WithOpName("float_caster"), original_image,
                               tf::DT_FLOAT);
      // The convention for image ops in TensorFlow is that all images are expected
      // to be in batches, so that they're four-dimensional arrays with indices of
      // [batch, height, width, channel]. Because we only have a single image, we
      // have to add a batch dimension of 1 to the start with ExpandDims().
      auto dims_expander = ExpandDims(root, float_caster, 0);
      
#if 0 // original example code to resise.
      
      // Bilinearly resize the image to fit the required dimensions.
      auto resized = ResizeBilinear(
                                    root, dims_expander,
                                    Const(root.WithOpName("size"), {input_height, input_width}));
      // Subtract the mean and divide by the scale.
      int32_t input_mean = 128;
      int32_t input_std = 128;
      Div(root.WithOpName(output_name), Sub(root, resized, {input_mean}),
          {input_std});
      
#else
      
      constexpr float kWidth = 259.0f;  // !!! Obviously an assumption, will be an arg to this or we crop when reading.
      constexpr float kHeight = 96.0f;  // !!!  "
      float normWidth = input_width / kWidth;
      float normHeight = input_height / kHeight;
      auto cropped = CropAndResize(root.WithOpName("cropped"), dims_expander,
       Const(root.WithOpName("boxes"), {{ 0.0f, 0.0f, normHeight, normWidth }}),
       // box_ind maps the boxes to which batch image they apply to.
       Const(root.WithOpName("box_ind"), { 0 }),
       Const(root.WithOpName("crop_size"), { input_height, input_width }));
      
#endif
      
      // This runs the GraphDef network definition that we've just constructed, and
      // returns the results in the output tensor.
      tf::GraphDef graph;
      TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));
      
      std::unique_ptr<tf::Session> session(tf::NewSession(tf::SessionOptions()));
      TF_RETURN_IF_ERROR(session->Create(graph));
      TF_RETURN_IF_ERROR(session->Run({}, {output_name, original_name}, {}, out_tensors));
      return Status::OK();
   }
}
