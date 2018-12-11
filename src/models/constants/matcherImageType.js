

export const matcherImageType = Object.freeze({
  aligned: 'Aligned',
  blended: 'Blended',
  sideBySideGood: 'Side by side good matches',
  sideBySideGoodInliers: 'Side by side inlier matches',
  selectionImage1: 'Align selection image 1',
  selectionImage2: 'Align selection image 2',
  keyPointsGoodFixed: 'Matched key points image 1',
  keyPointsGoodMoving: 'Matched Key points image 2',
  keyPointsInlierFixed: 'Inlier key points image 1',
  keyPointsInlierMoving: 'Inlier key points image 2'
});

export const defaultValue = matcherImageType.aligned;