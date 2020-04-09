#!/Users/Shared/anaconda3/bin/python

import imageio
import numpy as np

image1 = imageio.imread('/Users/jason/Desktop/Incubator1/ArtLogic - from Scott/Spectrograms_png/Cymbals/China Gong V1.wav.png')
image2 = imageio.imread('/Users/jason/Desktop/SpectroFromTensor.png')[:,:,1]

print("Image 1 max {}", image1.max())
print("Image 2 max {}", image2.max())

imageio.imwrite('/Users/jason/Desktop/DiffEq.png', np.equal(image2, image1).astype(np.uint8) * 255)

imageio.imwrite('/Users/jason/Desktop/Diff.png', np.abs(image2 - image1).astype(np.uint8))