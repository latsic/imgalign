
export const paramGroups = Object.freeze({
  other:                    { id: 1001, name: 'Other' },
  sift:                     { id: 1002, name: 'Sift' },
  surf:                     { id: 1003, name: 'Surf' },
  orb:                      { id: 1004, name: 'Orb' },
  brisk:                    { id: 1005, name: 'Brisk' },
  kaze:                     { id: 1006, name: 'Kaze' },
  akaze:                    { id: 1007, name: 'Akaze' },
  basic:                    { id: 1008, name: 'Basic' },
  compare:                  { id: 1009, name: 'Compare' },
  matchFilter:              { id: 1010, name: 'Match Filter' },
  logging:                  { id: 1011, name: 'Logging' },
  stitching:                { id: 1012, name: 'Stitching' },
  stitcherCommon:           { id: 1013, name: 'Common' },
  stitcherOrientation:      { id: 1014, name: 'Orientation' },
  stitcherProjection:       { id: 1015, name: 'Projection' },
  multiStitcher:            { id: 1016, name: 'Multi Stitcher'},
  multiStitcherCamera:      { id: 1017, name: 'Camera'},
  multiStitcherSeams:       { id: 1018, name: 'Seams'},
  multiStitcherImage:       { id: 1019, name: 'Image'},
  multiStitcherStitchOrder: { id: 1020, name: 'Stitch Order'},
  multiStitcherMemory:      { id: 1021, name: 'Memory'},
  multiStitcherConfidence:  { id: 1022, name: 'Confidence'}
});

const detTypeId = 200;
const desTypeId = 201;
const matcherTypeId = 202;
const tftTypeId = 203;
const compareDetTypesId = 301;
const compareImageTypeId = 302;
const projectionId = 303;
const projection2Id = 304;
const projection3Id = 305;
const bundleAdjustTypeId = 583;
const blendTypeId = 591;
const seamFinderTypeId = 577;

export const paramTypes = Object.freeze({

  imageCap: { id: 10, name: 'Image cap for detector', groupId: paramGroups.other.id },
  imageCapInput: { id: 11, name: 'Input images image cap', groupId: paramGroups.other.id },
  
  sift_featuresN: { id: 20, name: 'Max feature count', groupId: paramGroups.sift.id },
  sift_octaveLayersN: { id: 21, name: 'Number of octave layers', groupId: paramGroups.sift.id },
  sift_contrastThresh: { id: 22, name: 'Contrast threshold', groupId: paramGroups.sift.id },
  sift_edgeThresh: { id: 23, name: 'Edge threshold', groupId: paramGroups.sift.id },
  sift_sigma: { id: 24, name: 'Sigma', groupId: paramGroups.sift.id },
  
  surf_hessianThresh: { id: 30, name: 'Hessian threshold', groupId: paramGroups.surf.id },
  surf_octavesN: { id: 31, name: 'Number of octaves', groupId: paramGroups.surf.id },
  surf_octaveLayersN: { id: 32, name: 'Number of octave layers', groupId: paramGroups.surf.id },
  surf_extended: { id: 33, name: '64/128 bit descriptor', groupId: paramGroups.surf.id },

  orb_featuresN: { id: 40, name: 'Max feature count', groupId: paramGroups.orb.id },
  orb_scale: { id: 41, name: 'Scale', groupId: paramGroups.orb.id },
  orb_levelsN: { id: 42, name: 'Number of levels', groupId: paramGroups.orb.id },
  orb_edgeThresh: { id: 43, name: 'Edge threshold', groupId: paramGroups.orb.id },
  orb_patchSize: { id: 44, name: 'Patch size', groupId: paramGroups.orb.id},

  brisk_thresh: { id: 50, name: 'Threshold', groupId: paramGroups.brisk.id },
  brisk_octavesN: { id: 51, name: 'Number of octaves', groupId: paramGroups.brisk.id },
  brisk_patternScale: { id: 52, name: 'Pattern scale', groupId: paramGroups.brisk.id },

  kaze_thresh: { id: 53, name: 'Threshold', groupId: paramGroups.kaze.id },
  kaze_octavesN: { id: 54, name: 'Number of octaves', groupId: paramGroups.kaze.id },
  kaze_octaveLayersN: { id: 55, name: 'Number of octave layers', groupId: paramGroups.kaze.id },

  akaze_thresh: { id: 56, name: 'Threshold', groupId: paramGroups.akaze.id },
  akaze_octavesN: { id: 57, name: 'Number of octaves', groupId: paramGroups.akaze.id },
  akaze_octaveLayersN: { id: 58, name: 'Number of octave layers', groupId: paramGroups.akaze.id },

  detType_sift: { id: 60, name: 'Sift', groupId: detTypeId },
  detType_surf: { id: 61, name: 'Surf', groupId: detTypeId },
  detType_orb: { id: 62, name: 'Orb', groupId: detTypeId },
  detType_brisk: { id: 63, name: 'Brisk', groupId: detTypeId },
  detType_kaze: { id: 64, name: 'Kaze', groupId: detTypeId },
  detType_akaze: { id: 65, name: 'Akaze', groupId: detTypeId },

  desType_sift: { id: 80, name: 'Sift', groupId: desTypeId },
  desType_surf: { id: 81, name: 'Surf', groupId: desTypeId },
  desType_orb: { id: 82, name: 'Orb', groupId: desTypeId },
  desType_brisk: { id: 83, name: 'Brisk', groupId: desTypeId },
  desType_freak: { id: 84, name: 'Freak', groupId: desTypeId },
  desType_kaze: { id: 85, name: 'Kaze', groupId: desTypeId },
  desType_akaze: { id: 86, name: 'Akaze', groupId: desTypeId },

  matcherType_flann: { id: 100, name: 'Flann', groupId: matcherTypeId },
  matcherType_bfhamming: { id: 101, name: 'Brute force hamming', groupId: matcherTypeId },
  matcherType_bfhamming2: { id: 102, name: 'Brute force hamming 2', groupId: matcherTypeId },
  matcherType_auto: { id: 103, name: 'Auto', groupId: matcherTypeId },

  transformFinderType_ransac: { id: 120, name: 'Ransac', groupId: tftTypeId },
  transformFinderType_rho: { id: 121, name: 'Rho', groupId: tftTypeId },
  transformFinderType_lmeds: { id: 122, name: 'Lmeds', groupId: tftTypeId },

  detType: { id: detTypeId, name: 'Detector', groupId: paramGroups.basic.id },
  desType: { id: desTypeId, name: 'Descriptor', groupId: paramGroups.basic.id },
  matcherType: { id: matcherTypeId, name: 'Matcher', groupId: paramGroups.basic.id },
  transformFinderType: { id: tftTypeId, name: 'Transform finder', groupId: paramGroups.basic.id },

  compareDetTypes: { id: compareDetTypesId, name: 'Detectors', groupId: paramGroups.compare.id },
  compareImageType: { id: compareImageTypeId, name: 'Image', groupId: paramGroups.compare.id },

  imageTemplate: { id: 130, name: 'Template Image', groupId: compareImageTypeId },
  imageMoving: { id: 131, name: 'Moving Image', groupId: compareImageTypeId },

  matchFilterSpreadAuto: { id: 140, name: 'Spread factor automatic', groupId: paramGroups.matchFilter.id },
  matchFilterSpreadFactor: { id: 141, name: 'Spread factor', groupId: paramGroups.matchFilter.id, disabledIfId: 140},
  matchFilterMinMatchesToRetain: { id: 142, name: 'Min match count', groupId: paramGroups.matchFilter.id },
  matchFilterMaxMatchesToRetain: { id: 143, name: 'Max match count', groupId: paramGroups.matchFilter.id },

  floodFillTolerance: { id: 150, name: 'Flood fill tolerance', groupId: paramGroups.other.id },
  alignSelectionOverlay: { id: 151, name: 'Overlay selection', groupId: paramGroups.other.id },
  logInfoEnabled: { id: 152, name: 'Verbose logging', groupId: paramGroups.logging.id },
  logErrorEnabled: { id: 153, name: 'Log errors', groupId: paramGroups.logging.id },
  logAssertEnabled: { id: 154, name: 'Log assertions', groupId: paramGroups.logging.id },
  logExternEnabled: { id: 155, name: 'Log extern messages', groupId: paramGroups.logging.id },

  stitch_projection: { id: projectionId, name: 'Surface projection 1', groupId: paramGroups.stitcherProjection.id },
  stitch_projection2: { id: projection2Id, name: 'Surface projection 2', groupId: paramGroups.stitcherProjection.id },
  stitch_seamBlend: { id: 501, name: 'Seam blend', groupId: paramGroups.stitcherCommon.id },
  stitch_colorTransfer: { id: 502, name: 'Color transfer', groupId: paramGroups.stitcherCommon.id },
  
  stitch_fieldOfView1: { id: 511, name: 'Field of view 1', groupId: paramGroups.stitcherOrientation.id },
  stitch_yaw1: { id: 513, name: 'Yaw 1', groupId: paramGroups.stitcherOrientation.id },
  stitch_pitch1: { id: 514, name: 'Pitch 1', groupId: paramGroups.stitcherOrientation.id },
  
  stitch_fieldOfView2: { id: 521, name: 'Field of view 2', groupId: paramGroups.stitcherOrientation.id },
  stitch_yaw2: { id: 523, name: 'Yaw 2', groupId: paramGroups.stitcherOrientation.id },
  stitch_pitch2: { id: 524, name: 'Pitch 2', groupId: paramGroups.stitcherOrientation.id },
  
  stitch_yaw2Auto: { id: 525, name: 'Yaw 2 auto', groupId: null },
  stitch_pitch2Auto: { id: 526, name: 'Pitch 2 auto', groupId: null },

  stitch_projectionTypeNone: { id: 561, name: 'None', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeSpherical: { id: 553, name: 'Spherical', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id},
  stitch_projectionTypeCylindrical: { id: 552, name: 'Cylindrical', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeMercator: { id: 559, name: 'Mercator ', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeTransverseMercator: { id: 560, name: 'Transverse Mercator', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeFisheye: { id: 554, name: 'Fisheye', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypePlane: { id: 550, name: 'Plane', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeAffine: { id: 551, name: 'Affine', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeStereographic: { id: 555, name: 'Stereographic', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeRectilinear: { id: 556, name: 'Rectilinear', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeRectilinearPortrait: { id: 557, name: 'Rectilinear portrait', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypePanini: { id: 558, name: 'Panini', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeRectilinearA2B1: { id: 562, name: 'Rectilinear A2B1', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypeRectilinearPortraitA2B1: { id: 563, name: 'Rectilinear p. A2B1', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypePaniniA2B1: { id: 564, name: 'Panini A2B1', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },
  stitch_projectionTypePaniniPortraitA2B1: { id: 565, name: 'Panini portrait A2B1', groupId: projectionId, groupId2: projection2Id, groupId3: projection3Id },

  bundleAdjustType_none: { id: 570, name: 'None', groupId: bundleAdjustTypeId },
  bundleAdjustType_ray: { id: 571, name: 'Ray', groupId: bundleAdjustTypeId },
  bundleAdjustType_reprojCap: { id: 566, name: 'Reprojection Cap', groupId: bundleAdjustTypeId },
  bundleAdjustType_reproj: { id: 572, name: 'Reprojection', groupId: bundleAdjustTypeId },
  bundleAdjustType_auto: { id: 573, name: 'Auto', groupId: bundleAdjustTypeId },
  bundleAdjustType_rayretry: { id: 569, name: 'Ray retry', groupId: bundleAdjustTypeId },
  bundleAdjustType_ray2: { id: 568, name: 'Ray2', groupId: bundleAdjustTypeId },
  bundleAdjustType_reproj2: { id: 567, name: 'Reprojection2', groupId: bundleAdjustTypeId },
  bundleAdjustType_ray3: { id: 602, name: 'Ray3', groupId: bundleAdjustTypeId },
  

  blendType_none: { id: 574, name: 'None', groupId: blendTypeId },
  blendType_multiBand: { id: 575, name: 'Multiband', groupId: blendTypeId },
  blendType_feather: { id: 576, name: 'Feather', groupId: blendTypeId },

  
  seamFinderType_Vornoi: { id: 578, name: 'Vornoi', groupId: seamFinderTypeId },
  seamFinderType_Graphcut: { id: 579, name: 'Graphcut', groupId: seamFinderTypeId },

  multiStitch_projectionType: { id: projection3Id, name: 'Surface projection', groupId: paramGroups.multiStitcherCamera.id },
  multiStitch_bundleAdjust: { id: bundleAdjustTypeId, name: 'Bundle adjustement type', groupId: paramGroups.multiStitcherCamera.id },
  multiStitch_waveCorrection: { id: 584, name: 'Wave correction', groupId: paramGroups.multiStitcherCamera.id },
  multiStitch_camEstimate: { id: 582, name: 'Cam estimation', groupId: paramGroups.multiStitcherCamera.id },
  
 
  multiStitch_seamBlend: { id: 585, name: 'Seam blend', groupId: paramGroups.multiStitcherSeams.id },
  multiStitch_rectifyPerspective: { id: 581, name: 'Rectify perspective', groupId: paramGroups.multiStitcherImage.id },
  multiStitch_rectifyStretch: { id: 593, name: 'Rectify stretch', groupId: paramGroups.multiStitcherImage.id, enabledIfId: 581 },
  
  
  //multiStitch_blendType: { id: blendTypeId, name: 'Blend type', groupId: paramGroups.multiStitcherImage.id },
  multiStitch_blendStrength: { id: 592, name: 'Blend strength', groupId: paramGroups.multiStitcherSeams.id, enabledIfId: 585 },
  multiStitch_seamFinderType: { id: seamFinderTypeId, name: 'Seam finder', groupId: paramGroups.multiStitcherSeams.id },
  
  
  multiStitch_exposureCompensator: { id: 590, name: 'Exposure compens.', groupId: paramGroups.multiStitcherImage.id },
  multiStitch_colorTransfer: { id: 586, name: 'Color transfer', groupId: paramGroups.multiStitcherImage.id },

  multiStitch_calcImageOrder: { id: 587, name: 'Calc stitch order', groupId: paramGroups.multiStitcherStitchOrder.id },
  multiStitch_calcCenterImage: { id: 588, name: 'Estimate start image', groupId: paramGroups.multiStitcherStitchOrder.id },
  multiStitch_warpFirst: { id: 580, name: 'Warp first', groupId: paramGroups.multiStitcherStitchOrder.id },
  
  multiStitch_confidenceThreshCamManual: { id: 595, name: 'Thresh camera', groupId: 0 },
  multiStitch_confidenceThreshCam: { id: 594, name: 'Thresh camera', groupId: 0 },
  // multiStitch_confidenceThreshCamManual: { id: 595, name: 'Thresh camera', groupId: paramGroups.multiStitcherConfidence.id },
  // multiStitch_confidenceThreshCam: { id: 594, name: 'Thresh camera', groupId: paramGroups.multiStitcherConfidence.id, enabledIfId: 595 },
  multiStitch_confidenceThresh: { id: 589, name: 'Thresh stitching', groupId: paramGroups.multiStitcherConfidence.id },

  multiStitch_preserveAlphaChannelValue: { id: 600, name: 'Preserve Alpha Channel', groupId: paramGroups.multiStitcherMemory.id },
  multiStitch_disposeInputImages: { id: 598, name: 'Dispose input before stitching', groupId: paramGroups.multiStitcherMemory.id },
  multiStitch_limitResultPreview: { id: 596, name: 'Max result view size', groupId: paramGroups.multiStitcherMemory.id },
  multiStitch_limitInputView: { id: 597, name: 'Max input view size', groupId: paramGroups.multiStitcherMemory.id },
  multiStitch_limitLiveStitchingPreview: { id: 599, name: 'Max temp result size', groupId: paramGroups.multiStitcherMemory.id },
  multiStitch_liveUpdateCycle: { id: 601, name: 'Live update cycle', groupId: paramGroups.multiStitcherMemory.id }
  
});

export const paramTexts = Object.freeze([
  { id: paramTypes.sift_featuresN.id,
    text: 'The number of best features to retain (may include location duplicates). The features are ranked by their scores '
        + '(measured in SIFT algorithm as the local contrast). Value 0 => Automatic.'
  },
  { id: paramTypes.sift_octaveLayersN.id,
    text: 'The number of layers in each octave. 3 is the value used in D. Lowe paper. The '
        + 'number of octaves is computed automatically from the image resolution.'
  },
  { id: paramTypes.sift_contrastThresh.id,
    text: 'The contrast threshold used to filter out weak features in semi-uniform '
        + '(low-contrast) regions. The larger the threshold, the less features are produced by the detector.'
  },
  { id: paramTypes.sift_edgeThresh.id,
    text: 'The threshold used to filter out edge-like features. Note that the its meaning '
        + 'is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are '
        + 'filtered out (more features are retained).'
  },
  { id: paramTypes.sift_sigma.id,
    text: 'The sigma of the Gaussian applied to the input image at the octave. If your image '
        + 'is captured with a weak camera with soft lenses, you might want to reduce the number.'
  },

  { id: paramTypes.surf_hessianThresh.id,
    text: 'Threshold for hessian keypoint detector.'
  },
  { id: paramTypes.surf_octavesN.id,
    text: 'Number of pyramid octaves the keypoint detector will use.'
  },
  { id: paramTypes.surf_octaveLayersN.id,
    text: 'Number of octave layers within each octave.'
  },
  { id: paramTypes.surf_extended.id,
    text: 'On - use extended 128-element descriptors, off - use 64-element descriptors.'
  },

  { id: paramTypes.orb_featuresN.id,
    text: 'The maximum number of features to retain.'
  },
  { id: paramTypes.orb_scale.id,
    text: 'Pyramid decimation ratio, greater than 1. scaleFactor==2 means the classical '
        + 'pyramid, where each next level has 4x less pixels than the previous, but such a big scale factor '
        + 'will degrade feature matching scores dramatically. On the other hand, too close to 1 scale factor '
        + 'will mean that to cover certain scale range you will need more pyramid levels and so the speed '
        + 'will suffer.'
  },
  { id: paramTypes.orb_levelsN.id,
    text: 'The number of pyramid levels.'
  },
  { id: paramTypes.orb_edgeThresh.id,
    text: 'This is size of the border where the features are not detected. It should ' +
        + 'roughly match the patchSize parameter.'
  },
  { id: paramTypes.orb_patchSize.id,
    text: 'size of the patch used by the oriented BRIEF descriptor. Of course, on smaller '
        + 'pyramid layers the perceived image area covered by a feature will be larger.'
  },

  { id: paramTypes.brisk_thresh.id,
    text: 'AGAST detection threshold score.'
  },
  { id: paramTypes.brisk_octavesN.id,
    text: 'Detection octaves. Use 0 to do single scale.'
  },
  { id: paramTypes.brisk_patternScale.id,
    text: 'Apply this scale to the pattern used for sampling the neighbourhood of a keypoint.'
  },

  { id: paramTypes.kaze_thresh.id,
    text: 'Detector response threshold to accept point.'
  },
  { id: paramTypes.kaze_octavesN.id,
    text: 'Maximum octave evolution of the image.'
  },
  { id: paramTypes.kaze_octaveLayersN.id,
    text: 'Default number of sublevels per scale level.'
  },

  { id: paramTypes.akaze_thresh.id,
    text: 'Detector response threshold to accept point.'
  },
  { id: paramTypes.akaze_octavesN.id,
    text: 'Maximum octave evolution of the image.'
  },
  { id: paramTypes.akaze_octaveLayersN.id,
    text: 'Default number of sublevels per scale level.'
  },

  { id: paramTypes.matchFilterSpreadAuto.id,
    text: 'If enabled, the spread factor will be chosen automatically.'
  },
  { id: paramTypes.matchFilterSpreadFactor.id,
    text: 'When two detected features are compared, a match distance is calculated. '
        + 'The larger this distance, the less similar the features are. The spread factor '
        + 'is a value to influence how large the distance between two features can be, while '
        + 'the features are still considered to be a good match.'
  },
  { id: paramTypes.matchFilterMinMatchesToRetain.id,
    text: 'The minimum number of matches to retain (if possible). These matches are then used to align '
        + 'the image to the template image (calculation of the homography matrix).'
  },
  { id: paramTypes.matchFilterMaxMatchesToRetain.id,
    text: 'The maximum number of matches to retain. These matches are then used to align '
        + 'the image to the template image (calculation of the homography matrix).'
  },

  { id: paramTypes.floodFillTolerance.id,
    text: 'Tolerance value for the flood fill algorithm. After matching two images, '
        + ' and selecting one or multiple points in an input image, a flood fill image '
        + ' can be calculated via the select menu.'
  },
  { id: paramTypes.alignSelectionOverlay.id,
    text: 'Overlay the aligned selection onto the template image.'
  },
  { id: paramTypes.logInfoEnabled.id,
    text: 'Enabled or disable verbose console logging.'
  },
  { id: paramTypes.logErrorEnabled.id,
    text: 'Enabled or disable logging of errors.'
  },
  { id: paramTypes.logAssertEnabled.id,
    text: 'Enabled or disable logging of assertions.'
  },
  { id: paramTypes.logExternEnabled.id,
    text: 'Enabled or disable logging of extern messages.'
  },

  { id: paramTypes.imageCap.id,
    text: 'The maxium number of pixels for feature detection. '
        + 'If an image is larger it will be resized before the feature detection runs. '
        + 'This has an impact on the amount of time feature detection consumes.'
  },
  { id: paramTypes.imageCapInput.id,
    text: 'The maxium size of input images in pixels'
  },

  { id: paramTypes.stitch_projection.id,
    text: 'Surface projection for template image.'
  },
  { id: paramTypes.stitch_projection2.id,
    text: 'Surface projection for image to align.'
  },
  { id: paramTypes.stitch_seamBlend.id,
    text: 'Blend seam between images.'
  },
  { id: paramTypes.stitch_colorTransfer.id,
    text: 'Try to adjust the colors of the image to align '
        + 'to the colors of the templte image. Does not work for black and white images.'
  },
  { id: paramTypes.stitch_fieldOfView1.id,
    text: 'Field of view in the degrees of the template image. '
        + 'Leave empty to use default value or data from image meta data if available.'
  },
  { id: paramTypes.stitch_yaw1.id,
    text: 'Horizontal orientation of template image in degrees. '
        + 'Leave empty to use default value.'
  },
  { id: paramTypes.stitch_pitch1.id,
    text: 'Vertical orientation of template image in degrees. '
        + 'Leave empty to use default value.'
  },
  { id: paramTypes.stitch_fieldOfView2.id,
    text: 'Field of view in the degrees of the image to align. '
        + 'Leave empty to use default value or data from image meta data if available.'
  },
  { id: paramTypes.stitch_yaw2.id,
    text: 'Horizontal orientation of the image to align in degrees. '
        + 'Leave empty to use default value.'
  },
  { id: paramTypes.stitch_pitch2.id,
    text: 'Vertical orientation of the image to align in degrees. '
        + 'Leave empty to use default value.'
  },
  { id: paramTypes.stitch_yaw2Auto.id,
    text: 'Unused.'
  },
  { id: paramTypes.stitch_pitch2Auto.id,
    text: 'Unused.'
  },

  { id: paramTypes.multiStitch_projectionType.id,
    text: 'Surface projection.'
  },
  { id: paramTypes.multiStitch_bundleAdjust.id,
    text: 'Bundle adjustement type. Automatically refines initial camera parameters.'
  },
  { id: paramTypes.multiStitch_camEstimate.id,
    text: 'Tries to guess camera paramters (focal length and rotation)'
      + 'Rotation parameters are only used if bundle adjustement is enabled.'
  },
  { id: paramTypes.multiStitch_waveCorrection.id,
    text: 'Tries to straighten a curved panorama/horizon.'
  },
  // { id: paramTypes.multiStitch_seamBlend.id,
  //   text: 'Blend seam between stitched images.'
  // },
  { id: paramTypes.multiStitch_colorTransfer.id,
    text: 'Tries to adjust the colors of the images with the purpose that '
        + 'all images have the same color tone. The first image in the '
        + 'stitching pipeline is used as template. This usually does not work with '
        + 'with black and white images.'
  },
  { id: paramTypes.multiStitch_calcImageOrder.id,
    text: 'Calculate the best stitching order instead of stitching the images in the same order as the '
        + 'input images are displayed.' 
  },
  { id: paramTypes.multiStitch_calcCenterImage.id,
    text: 'Calculate a center image and start stitching from there.'
  },
  { id: paramTypes.multiStitch_confidenceThresh.id,
    text: 'When calculating a mapping between two images, a confidence value '
        + 'indicates how good the mapping is. Mappings with a confidence value lower '
        + 'than this value are dismissed.' 
  },

  { id: paramTypes.multiStitch_limitResultPreview.id,
    text: 'To save memory, the display stitched result image may be shown only in a reduced resolution. '
        + 'The full resolution image can still be saved via the "save image button".' 
  },
  { id: paramTypes.multiStitch_limitInputView.id,
    text: 'To save memory, the displayed input images may be resized and displayed with a lower resolution. '
        + 'This has no influence on the final stitching result.'
  },
  { id: paramTypes.multiStitch_disposeInputImages.id,
    text: 'To save memory, before the stitching process is started, all input '
        + 'images are disposed and only a small preview is kept. This has no influence '
        + 'on the final stitching result. If the stitching process is restarted with the same images though, '
        + 'the images will have to be reloaded (this is done automatically).' 
  },
  { id: paramTypes.multiStitch_limitLiveStitchingPreview.id,
    text: 'To save memory and increase performance, the preview during stitching can be reduces in size.'
        + 'This has no influence on the final stitching result.' 
  },
  { id: paramTypes.multiStitch_preserveAlphaChannelValue.id,
    text: 'To save memory while blending, preserving of alpha channel values can be disabled.' 
        + 'The final stitching result is only influeces if images containing non opaque areas are stitched together.' 
  }



  


]);

export class ParamUtils {
  static _getParam(id) {
    const paramKey = Object.keys(paramTypes).find(key => {
      return paramTypes[key].id == id;
    });
    if(!paramKey) {
      throw new Error(`No param with id ${id} exists`);
    }
    return paramTypes[paramKey];
  }

  static getParamName(id) {
    return this._getParam(id).name;
  }

  static getParamGroupId(id) {
    return this._getParam(id).groupId;
  }

  static getParamText(id) {
    const paramTextObj = paramTexts.find(paramTextObj => {
      return paramTextObj.id == id;
    });
    if(paramTextObj) {
      return paramTextObj.text;
    }
    return '';
  }

  static getParamEnabledIfId(id) {
    const param = this._getParam(id);
    return param && param.enabledIfId
      ? param.enabledIfId
      : null;
  }

  static getParamDisabledIfId(id) {
    const param = this._getParam(id);
    return param && param.disabledIfId
      ? param.disabledIfId
      : null;
  }
}

export const valueTypes = Object.freeze({
  discrete: 1,
  multiple: 2,
  range: 3,
  rangeSquareRoot: 4,
  number: 5,
  bool: 6,
  numberAuto: 7
});

