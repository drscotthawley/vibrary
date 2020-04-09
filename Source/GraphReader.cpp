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

#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/cc/saved_model/tag_constants.h"

#include "GraphReader.h"

namespace tf = tensorflow;

#define qTraceTensorFlow 0

bool AGraphReader::ReadModel(const std::string& graphFilename, SavedModel& bundle)
{
   tf::RunOptions runOpts;
#if qTraceTensorFlow
   runOpts.set_trace_level(tf::RunOptions::TraceLevel_MAX);
#endif
   // The saved model tag serve comes from the server script that saved the model
   // panotti/utils/h5topb.py
   const auto status = tf::LoadSavedModel(tf::SessionOptions(), runOpts, graphFilename,
                                          { tf::kSavedModelTagServe }, &bundle);
   if (!status.ok())
   {
      throw std::runtime_error(status.error_message());
   }
   return status.ok();
}
