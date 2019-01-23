
const msgLoad = 'load';
const msgMatch = 'match';
const msgParams = 'params';
const msgInit = 'init';
const msgReset = 'reset';
const msgAlignedImage = 'alignedImage';
const msgBlendedImage = 'blendedImage';
const msgStitchedImage = 'stitchedImage';
const msgStitchedImageWithKeyPoints = 'stitchedImageWithKeyPoints';
const msgBlendedImagePolygonFixedImage = 'blendedImagePolygonFixedImage';
const msgBlendedImagePolygonMovingImage = 'blendedImagePolygonMovingImage';
const msgBlendedImageFloodFillFixedImage = 'blendedImageFloodFillFixedImage';
const msgBlendedImageFloodFillMovingImage = 'blendedImageFloodFillMovingImage';
const msgSideBySideImageGood = 'sideBySideImageGood';
const msgSideBySideImageGoodInliers = 'sideBySideImageGoodInliers';
const msgKeyPointsFixedImageGood = 'keyPointsFixedImageGood';
const msgKeyPointsMovingImageGood = 'keyPointsMovingImageGood';
const msgKeyPointsFixedImageInlier = 'keyPointsFixedImageInlier';
const msgKeyPointsMovingImageInlier = 'keyPointsMovingImageInlier';
const msgDetCompareImage = 'detCompareImage';
const msgConsoleInfo = 'cInfo';
const msgConsoleError = 'cError';
const msgConsoleLog = 'cLog';
const msgConsoleAssert = 'cAssert';
const msgResize = 'resize';
const msgStitch = 'stitch';

const msgMultiStitchInit = 'multiStitchInit';
const msgMultiStitchReset = 'multiStitchReset';
const msgMultiStitchParams = 'multiStitchParams';
const msgMultiStitch = 'multiStitch';

const msgMultiStitchStart = 'multiStitchStart';
const msgMultiStitchNext = 'multiStitchNext';

let mFixedMat = null;
let mMovingMat = null;
let mImgAlign = null;

let mTransMat = null;
let mFixedPtsGood = null;
let mMovingPtsGood = null;
let mFixedPtsInlier = null;
let mMovingPtsInlier = null;

let mImgStitch = null;
let mMultiStitchImages = null;


function postMessageConsoleFromExtern(msg, args) {
  postMessage({
    msg: msg,
    info: 'extern',
    payload: Array.from(args)
  });
}

function consoleLog() {
  postMessage({
    msg: msgConsoleLog,
    payload: Array.from(arguments)
  });
}

// eslint-disable-next-line no-console
console.log = function() { postMessageConsoleFromExtern(msgConsoleLog, arguments) }
// eslint-disable-next-line no-console
console.info = function() { postMessageConsoleFromExtern(msgConsoleInfo, arguments) };
// eslint-disable-next-line no-console
console.error = function() { postMessageConsoleFromExtern(msgConsoleError, arguments) };
// eslint-disable-next-line no-console
console.assert = function() { postMessageConsoleFromExtern(msgConsoleAssert, arguments) };


function imageDataFromMat(mat) {

  if(!(mat instanceof cv.Mat)) {
    throw new Error('not a valid opencv Mat instance');
  }

  if(mat.rows == 0 || mat.cols == 0) {
    return null;
  }

  // convert the mat type to cv.CV_8U
  const img = new cv.Mat();
  const depth = mat.type() % 8;
  const scale = depth <= cv.CV_8S? 1.0 : (depth <= cv.CV_32S? 1.0/256.0 : 255.0);
  const shift = (depth === cv.CV_8S || depth === cv.CV_16S)? 128.0 : 0.0;
  mat.convertTo(img, cv.CV_8U, scale, shift);

  // convert the img type to cv.CV_8UC4
  switch (img.type()) {
      case cv.CV_8UC1:
          cv.cvtColor(img, img, cv.COLOR_GRAY2RGBA);
          break;
      case cv.CV_8UC3:
          cv.cvtColor(img, img, cv.COLOR_RGB2RGBA);
          break;
      case cv.CV_8UC4:
          break;
      default:
          throw new Error('Bad number of channels (Source image must have 1, 3 or 4 channels)');
          // return;
  }
  return new ImageData(new Uint8ClampedArray(img.data), img.cols, img.rows);
}

function postDoneMessage(msg, payload = null) {
  postMessage({
    msg: msg,
    info: 'done',
    payload: payload
  });
}

function postDoneMessageImage(msg, imageData, additionalData) {
  
  if(!imageData) {
    postMessage({
      msg: msg,
      info: 'done',
      payload: {
        width: 0,
        height: 0,
        bufLen: 0,
        bufOffset: 0,
        buf: null,
        additionalData: additionalData
      }
    });
  }
  else {
    postMessage({
      msg: msg,
      info: 'done',
      payload: {
        width: imageData.width,
        height: imageData.height,
        bufLen: imageData.data.length,
        bufOffset: imageData.data.byteOffset,
        buf: imageData.data.buffer,
        additionalData: additionalData
      }
    }, [ imageData.data.buffer ]);
  }
}

function _reset(resetResults = true) {

  if(mImgAlign) {
    mImgAlign.delete();
    mImgAlign = null;
  }
  if(mFixedMat) {
    mFixedMat.delete();
    mFixedMat = null;
  }
  if(mMovingMat) {
    mMovingMat.delete();
    mMovingMat = null;
  }

  if(resetResults) {
    if(mTransMat) {
      mTransMat.delete();
      mTransMat = null;
    }
    if(mFixedPtsGood) {
      mFixedPtsGood.delete();
      mFixedPtsGood = null;
    }
    if(mMovingPtsGood) {
      mMovingPtsGood.delete();
      mMovingPtsGood = null;
    }
    if(mFixedPtsInlier) {
      mFixedPtsInlier.delete();
      mFixedPtsInlier = null;
    }
    if(mMovingPtsInlier) {
      mMovingPtsInlier.delete();
      mMovingPtsInlier = null;
    }
  }
}

function _multiStitchReset()
{
  if(mImgStitch) {
    mImgStitch.delete();
    mImgStitch = null;
  }
  if(mMultiStitchImages) {
    mMultiStitchImages.delete();
    mMultiStitchImages = null;
  }
}

function reset(resetResults = true) {
  _reset(resetResults);
  return () => postDoneMessage(msgReset);
}

function multiStitchReset() {
  _multiStitchReset();
  return () => postDoneMessage(msgMultiStitchReset);
}

function init(payload) {
  
  if(!cv || !cv.Mat) {
    throw new Error('No data available, forgot to call load?');
  }

  if(!payload.fixedImage || !payload.movingImage) {
    throw new Error('invalid input data');
  }
  if(payload.fixedImage.width <= 0 || payload.fixedImage.height <= 0) {
    throw new Error('Fixed image has invalid size');
  }
  if(payload.movingImage.width <= 0 || payload.movingImage.height <= 0) {
    throw new Error('Moving image has invalid size');
  }

  _reset(false);

  mFixedMat = cv.matFromImageData(payload.fixedImage);
  mMovingMat = cv.matFromImageData(payload.movingImage);
  try {
    mImgAlign = new cv.ImgAlign(mFixedMat, mMovingMat);
  }
  catch(error) {
    _reset(false);
    throw error;
  }
  return () => postDoneMessage(msgInit);
}

function multiStitchInit(payload) {
  if(!cv || !cv.Mat) {
    throw new Error('No data available, forgot to call load?');
  }

  if(mImgStitch == null) {

    _multiStitchReset();
    try {

      mMultiStitchImages = new cv.MatVector();
      for(const image of payload.images) {
        mMultiStitchImages.push_back(cv.matFromImageData(image));
      }   

      mImgStitch = new cv.ImgStitch(mMultiStitchImages);
      mMultiStitchImages.delete();
      mMultiStitchImages = null;
    }
    catch(error) {
      _multiStitchReset();
      throw error;
    }
  }
  return () => postDoneMessage(msgMultiStitchInit);
}

function _setParams(paramsArr, dstInstance) {

  if(paramsArr && paramsArr.length > 0) {
   
    let paramTypes = new cv.IntVector();
    let paramValues = new cv.FloatVector();

    paramsArr.forEach(param => {
      paramTypes.push_back(param.id);
      paramValues.push_back(param.value);
    });

    try{
      dstInstance.set(paramTypes, paramValues);
    }
    finally{
      paramTypes.delete();
      paramValues.delete();
    }
  }
}

function setParams(paramsArr) {

  if(!mImgAlign) {
    throw new Error('No instance available, forgot to call init?');
  }

  _setParams(paramsArr, mImgAlign);

  return () => postDoneMessage(msgParams);
}

function multiStitchSetParams(paramsArr) {
  if(!mImgStitch) {
    throw new Error('No instance available, forgot to call init?');
  }
  _setParams(paramsArr, mImgStitch);

  return () => postDoneMessage(msgMultiStitchParams);

}

function compare(payload) {
  if(!mImgAlign) {
    throw new Error('No instance available, forgot to call init?');
  }

  let sideBySideImage = new cv.Mat();
  let detTypes = new cv.IntVector();
  let timeUsed = new cv.IntVector();
  let keyPointsN = new cv.IntVector();
  
  for(let i = 0; i < payload.detTypesArr.length; ++i) {
    detTypes.push_back(payload.detTypesArr[i]);
  }

  const polys = [
    { v: new cv.PointVector(),
      src: payload && payload.fixedImagePolygonPts ? payload.fixedImagePolygonPts : null
    },
    { v: new cv.PointVector(),
      src: payload && payload.movingImagePolygonPts ? payload.movingImagePolygonPts : null
    },
  ];
  for(const poly of polys) {
    if(!poly.src) continue;
    for(let i = 0; i < poly.src.length; ++i) {
      poly.v.push_back({ x: poly.src[i].x, y: poly.src[i].y });
    }
  }

  try {
    mImgAlign.compare(
      payload.imageType, detTypes, sideBySideImage, timeUsed, keyPointsN,
      polys[0].v, polys[1].v);

    const imageData = imageDataFromMat(sideBySideImage);

    const jsTimeUsed = [];
    for(let i = 0; i < timeUsed.size(); ++i) {
      jsTimeUsed.push(timeUsed.get(i))
    }

    const jsKeyPointsN = [];
    for(let i = 0; i < keyPointsN.size(); ++i) {
      jsKeyPointsN.push(keyPointsN.get(i));
    }

    return () => postDoneMessageImage(
      msgDetCompareImage, imageData, {
        timeUsedArr: jsTimeUsed,
        keyPointsCountArr: jsKeyPointsN
      }
    );
  }
  finally {
    detTypes.delete();
    timeUsed.delete();
    sideBySideImage.delete();

    for(const poly of polys) {
      poly.v.delete();
    }
  }
}

function stitch(payload) {
  throwIfImageNotAvailable();

  let stitchedImage = new cv.Mat();

  try {
    const fieldOfView = mImgAlign.stitch(
      mTransMat,
      payload.projectionType1,
      payload.projectionType2,
      payload.seamBlend,
      payload.colorTransfer,
      payload.fieldOfViewFixedImage,
      payload.fieldOfViewMovingImage,
      payload.calcRotationYaw2,
      payload.calcRotationPitch2,
      payload.yaw1,
      payload.pitch1,
      payload.yaw2,
      payload.pitch2,
      stitchedImage
    );
    
    const imageData = imageDataFromMat(stitchedImage);

    return () => postDoneMessageImage(
      msgStitch, imageData, fieldOfView);
  }
  finally {
    stitchedImage.delete();
  }

}

function multiStitch(payload) {
  if(!mImgStitch) {
    throw new Error('No instance available, forgot to call init?');
  }

  let fieldsOfView = new cv.FloatVector();
  let stitchedImage = new cv.Mat();

  try {

    for(let fieldOfView of payload.fieldsOfView) {
      fieldsOfView.push_back(fieldOfView);
    }
    
    const stitchedImagesN = mImgStitch.stitch(fieldsOfView, stitchedImage);    
    const imageData = imageDataFromMat(stitchedImage);

    return () => postDoneMessageImage(msgMultiStitch, imageData, stitchedImagesN);
  }
  finally {
    fieldsOfView.delete();
    stitchedImage.delete();
  }
}

function multiStitchStart(payload) {
  if(!mImgStitch) {
    throw new Error('No instance available, forgot to call init?');
  }

  let fieldsOfView = new cv.FloatVector();
  let stitchIndices = new cv.IntVector();
  let stitchedImage = new cv.Mat();
  try {

    for(let fieldOfView of payload.fieldsOfView) {
      fieldsOfView.push_back(fieldOfView);
    }
    
    mImgStitch.stitchStart(fieldsOfView, stitchedImage, stitchIndices); 
    
    const stitchIndicesResult = [];
    for(let i = 0; i < stitchIndices.size(); ++i) {
      stitchIndicesResult.push(stitchIndices.get(i));
    }

    const imageData = imageDataFromMat(stitchedImage);

    return () => postDoneMessageImage(msgMultiStitchStart, imageData, stitchIndicesResult);
  }
  finally {
    fieldsOfView.delete();
    stitchedImage.delete();
    stitchIndices.delete();
  }
}

function multiStitchNext() {
  if(!mImgStitch) {
    throw new Error('No instance available, forgot to call init?');
  }

  let stitchedImage = new cv.Mat();

  try {
    const imagesN = mImgStitch.stitchNext(stitchedImage);

    if(imagesN == -1) {
      throw new Error("failed to stitch image");
    }

    if(imagesN == 0) {
      // throw new Error("stitching was already finished");
      // eslint-disable-next-line no-console
      console.warn("[webworker_matcher][multiStitchNext][info]", "failed to stitch all images");
    }

    const imageData = imageDataFromMat(stitchedImage);
    return () => postDoneMessageImage(msgMultiStitchNext, imageData, imagesN);
  }
  finally {
    stitchedImage.delete();
  }
}

function match(payload) {

  if(!mImgAlign) {
    throw new Error('No instance available, forgot to call init?');
  }

  if(!mTransMat) {
    mTransMat = new cv.Mat();
  }
  if(!mFixedPtsGood) {
    mFixedPtsGood = new cv.PointVector();
  }
  if(!mMovingPtsGood) {
    mMovingPtsGood = new cv.PointVector();
  }
  if(!mFixedPtsInlier) {
    mFixedPtsInlier = new cv.PointVector();
  }
  if(!mMovingPtsInlier) {
    mMovingPtsInlier = new cv.PointVector();
  }

  const polys = [
    { v: new cv.PointVector(),
      src: payload && payload.fixedImagePolygonPts ? payload.fixedImagePolygonPts : null
    },
    { v: new cv.PointVector(),
      src: payload && payload.movingImagePolygonPts ? payload.movingImagePolygonPts : null
    },
  ];
  for(const poly of polys) {
    if(!poly.src) continue;
    for(let i = 0; i < poly.src.length; ++i) {
      poly.v.push_back({ x: poly.src[i].x, y: poly.src[i].y });
    }
  }

  try {
    const success = mImgAlign.match_getExtData(
      polys[0].v, polys[1].v,
      mTransMat, mFixedPtsGood, mMovingPtsGood, mFixedPtsInlier, mMovingPtsInlier);

    return () => postDoneMessage(msgMatch, success);
  }
  finally {
    for(const poly of polys) {
      poly.v.delete();
    }
  }
}

function throwIfImageNotAvailable(keyPoints = false) {
  if(!mImgAlign) {
    throw new Error('No data available, forgot to call init?');
  }
  if(!mTransMat) {
    throw new Error('No data available, forgot to call match?');
  }
  if(keyPoints && (!mFixedPtsGood || !mMovingPtsGood || !mFixedPtsInlier || !mMovingPtsInlier) ) {
    throw new Error('No data available, forgot to call match?');
  }
}

function resize(payload) {
  
  let image = cv.matFromImageData(payload.image);
  let resizeImage = new cv.Mat();
  let dims = new cv.Size(payload.width, payload.height);
  try {
    cv.resize(image, resizeImage, dims, 0, 0, 3);
    const imageData = imageDataFromMat(resizeImage);
    return () => postDoneMessageImage(msgResize, imageData);
  }
  finally {
    image.delete();
    resizeImage.delete();
  }
}

function alignedImage() {
  throwIfImageNotAvailable();

  let alignedImage = new cv.Mat();
  try{
      mImgAlign.getImageAligned(mTransMat, alignedImage);
      const imageData = imageDataFromMat(alignedImage);
      return () => postDoneMessageImage(msgAlignedImage, imageData);
  }
  finally {
    alignedImage.delete();
  }
}

function blendedImage(payload) {
  throwIfImageNotAvailable();

  let blendedImage = new cv.Mat();
  try {
    mImgAlign.getImageBlended(mTransMat, payload.weight, payload.doOverlay, blendedImage);
    const imageData = imageDataFromMat(blendedImage);
    return () => postDoneMessageImage(msgBlendedImage, imageData);
  }
  finally {
    blendedImage.delete();
  }
}

function stitchedImage(payload) {
  throwIfImageNotAvailable();

  let stitchedImage = new cv.Mat();
  let pts = new cv.PointVector();

  try {
    mImgAlign.getStitchedImage(mTransMat, pts, payload.weight, stitchedImage);
    const imageData = imageDataFromMat(stitchedImage);
    return () => postDoneMessageImage(msgStitchedImage, imageData);
  }
  finally {
    stitchedImage.delete();
    pts.delete();
  }
}

function stitchedImageWithKeyPoints(payload) {
  throwIfImageNotAvailable();

  let stitchedImage = new cv.Mat();

  try {
    mImgAlign.getStitchedImage(mTransMat, mFixedPtsInlier, payload.weight, stitchedImage);
    const imageData = imageDataFromMat(stitchedImage);
    return () => postDoneMessageImage(msgStitchedImageWithKeyPoints, imageData);
  }
  finally {
    stitchedImage.delete();
  }
}

function blendedImagePolygon(payload) {
  throwIfImageNotAvailable();

  let blendedImage = new cv.Mat();
  let polygon = new cv.PointVector();
  try {

    for(let i = 0; i < payload.polygon.length; ++i) {
      polygon.push_back({ x: payload.polygon[i].x, y: payload.polygon[i].y });
    }

    if(payload.isFixedImage) {

      mImgAlign.getImageBlendedPolygonFixedImage(
        mTransMat, payload.weight, payload.doOverlay, polygon, blendedImage);
    }
    else {
      mImgAlign.getImageBlendedPolygonMovingImage(
        mTransMat, payload.weight, payload.doOverlay, polygon, blendedImage);
    }

    const imageData = imageDataFromMat(blendedImage);

    return () => postDoneMessageImage(
      payload.isFixedImage
        ? msgBlendedImagePolygonFixedImage
        : msgBlendedImagePolygonMovingImage,
        imageData);
  }
  finally {
    blendedImage.delete();
    polygon.delete();
  }
}

function blendedImageFloodFill(payload) {
  throwIfImageNotAvailable();

  let blendedImage = new cv.Mat();
  let pts = new cv.PointVector();
  let ffTolerances = new cv.IntVector();

  try {

    for(let i = 0; i < payload.pts.length; ++i) {
      pts.push_back({ x: payload.pts[i].x, y: payload.pts[i].y });
    }

    for(let i = 0; i < payload.floodFillTolerances.length; ++i) {
      ffTolerances.push_back(payload.floodFillTolerances[i]);
    }
   
    if(payload.isFixedImage) {

      mImgAlign.getImageFloodFillFixedImage(
        mTransMat, pts, ffTolerances, payload.weight, payload.doOverlay, blendedImage);
    }
    else {
      mImgAlign.getImageFloodFillMovingImage(
        mTransMat, pts, ffTolerances, payload.weight, payload.doOverlay, blendedImage);
    }

    const imageData = imageDataFromMat(blendedImage);

    return () => postDoneMessageImage(
      payload.isFixedImage
        ? msgBlendedImageFloodFillFixedImage
        : msgBlendedImageFloodFillMovingImage,
        imageData);
  }
  finally {
    blendedImage.delete();
    pts.delete();
    ffTolerances.delete();
  }
}

function sideBySideImageGood() {
  throwIfImageNotAvailable();

  let sideBySideImage = new cv.Mat();
  try {
    mImgAlign.getSideBySideImage(mFixedPtsGood, mMovingPtsGood, sideBySideImage);
    const imageData = imageDataFromMat(sideBySideImage);
    return () => postDoneMessageImage(msgSideBySideImageGood, imageData);
  }
  finally {
    sideBySideImage.delete();
  }
}

function sideBySideImageGoodInliers() {
  throwIfImageNotAvailable();

  let sideBySideImage = new cv.Mat();
  try {
    mImgAlign.getSideBySideImage(mFixedPtsInlier, mMovingPtsInlier, sideBySideImage);
    const imageData = imageDataFromMat(sideBySideImage);
    return () => postDoneMessageImage(msgSideBySideImageGoodInliers, imageData);
  }
  finally {
    sideBySideImage.delete();
  }
}

function keyPointsFixedImageGood() {
  throwIfImageNotAvailable(true);

  let keyPointsFixedImage = new cv.Mat();
  try {
    mImgAlign.getFixedImageWithMatchedPoints(mFixedPtsGood, keyPointsFixedImage);
    const imageData = imageDataFromMat(keyPointsFixedImage);
    return () => postDoneMessageImage(msgKeyPointsFixedImageGood, imageData);
  }
  finally {
    keyPointsFixedImage.delete();
  }
}

function keyPointsMovingImageGood() {
  throwIfImageNotAvailable(true);

  let keyPointsMovingImage = new cv.Mat();
  try {
    mImgAlign.getMovingImageWithMatchedPoints(mMovingPtsGood, keyPointsMovingImage);
    const imageData = imageDataFromMat(keyPointsMovingImage);
    return () => postDoneMessageImage(msgKeyPointsMovingImageGood, imageData);
  }
  finally {
    keyPointsMovingImage.delete();
  }
}

function keyPointsFixedImageInlier() {
  throwIfImageNotAvailable(true);

  let keyPointsFixedImage = new cv.Mat();
  try {
    mImgAlign.getFixedImageWithMatchedPoints(mFixedPtsInlier, keyPointsFixedImage);
    const imageData = imageDataFromMat(keyPointsFixedImage);
    return () => postDoneMessageImage(msgKeyPointsFixedImageInlier, imageData);
  }
  finally {
    keyPointsFixedImage.delete();
  }
}

function keyPointsMovingImageInlier() {
  throwIfImageNotAvailable(true);

  let keyPointsMovingImage = new cv.Mat();
  try {
    mImgAlign.getMovingImageWithMatchedPoints(mMovingPtsInlier, keyPointsMovingImage);
    const imageData = imageDataFromMat(keyPointsMovingImage);
    return () => postDoneMessageImage(msgKeyPointsMovingImageInlier, imageData);
  }
  finally {
    keyPointsMovingImage.delete();
  }
}

function waitForOpencv(callbackFn, waitTimeMs = 30000, stepTimeMs = 100) {

  if(cv.Mat) callbackFn(true);

  // eslint-disable-next-line no-console
  // console.log('[webworker_matcher]', 'Waiting for OpenCV to be fully loaded...');
  consoleLog('[webworker_matcher]', 'Waiting for OpenCV to be fully loaded...');

  let timeSpentMs = 0;
  const intervalId = setInterval(() => {
    
    const limitReached = timeSpentMs > waitTimeMs;
    if(cv.Mat || limitReached) {
      clearInterval(intervalId);
      return callbackFn(!limitReached);
    }
    else {
      timeSpentMs += stepTimeMs;
    }
  }, stepTimeMs);
}

onmessage = function(e) {
  // eslint-disable-next-line no-console
  // console.log(`[webworker_matcher][received][msg][${e.data.msg}]`);
  consoleLog(`[webworker_matcher][received][msg][${e.data.msg}]`);

  let startTime = new Date();
  let postMessageFn = null;

  switch(e.data.msg) {
    case msgLoad:
      self.importScripts('./opencv_3_4_custom.js');
      // eslint-disable-next-line no-console
      // console.log('[webworker_matcher][importScripts][opencv.js][ms]', new Date() - startTime);
      consoleLog('[webworker_matcher][importScripts][opencv.js][ms]', new Date() - startTime);
      // wait for opencv to be really ready!
      waitForOpencv(function(success){
        if(success) postDoneMessage(e.data.msg);
        else throw new Error('Failed to load OpeCV: time out');
      });
      break;
    case msgInit: {
      postMessageFn = init(e.data.payload);
      break;
    }
    case msgParams: {
      postMessageFn = setParams(e.data.payload);
      break;
    }
    case msgMatch: {
      postMessageFn = match(e.data.payload);
      break;
    }
    case msgReset: {
      postMessageFn = reset();
      break;
    }
    case msgAlignedImage: {
      postMessageFn = alignedImage();
      break;
    }
    case msgBlendedImage: {
      postMessageFn = blendedImage(e.data.payload);
      break;
    }
    case msgStitchedImage: {
      postMessageFn = stitchedImage(e.data.payload);
      break;
    }
    case msgStitchedImageWithKeyPoints: {
      postMessageFn = stitchedImageWithKeyPoints(e.data.payload);
      break;
    }
    case msgBlendedImagePolygonFixedImage:
    case msgBlendedImagePolygonMovingImage: {
      postMessageFn = blendedImagePolygon(e.data.payload);
      break;
    }
    case msgBlendedImageFloodFillFixedImage:
    case msgBlendedImageFloodFillMovingImage: {
      postMessageFn = blendedImageFloodFill(e.data.payload);
      break;
    }
    case msgSideBySideImageGood: {
      postMessageFn = sideBySideImageGood();
      break;
    }
    case msgSideBySideImageGoodInliers: {
      postMessageFn = sideBySideImageGoodInliers();
      break;
    }
    case msgKeyPointsFixedImageGood: {
      postMessageFn = keyPointsFixedImageGood();
      break;
    }
    case msgKeyPointsMovingImageGood: {
      postMessageFn = keyPointsMovingImageGood();
      break;
    }
    case msgKeyPointsFixedImageInlier: {
      postMessageFn = keyPointsFixedImageInlier();
      break;
    }
    case msgKeyPointsMovingImageInlier: {
      postMessageFn = keyPointsMovingImageInlier();
      break;
    }
    case msgDetCompareImage: {
      postMessageFn = compare(e.data.payload);
      break;
    }
    case msgStitch: {
      postMessageFn = stitch(e.data.payload);
      break;
    }
    case msgResize: {
      postMessageFn = resize(e.data.payload);
      break;
    }

    case msgMultiStitchInit: {
      postMessageFn = multiStitchInit(e.data.payload);
      break;
    }
    case msgMultiStitchReset: {
      postMessageFn = multiStitchReset();
      break;
    }
    case msgMultiStitchParams: {
      postMessageFn = multiStitchSetParams(e.data.payload);
      break;
    }
    case msgMultiStitch: {
      postMessageFn = multiStitch(e.data.payload);
      break;
    }

    case msgMultiStitchStart: {
      postMessageFn = multiStitchStart(e.data.payload);
      break;
    }
    case msgMultiStitchNext: {
      postMessageFn = multiStitchNext();
      break;
    }

    default:
      break;
  }

  // eslint-disable-next-line no-console
  // console.log(`[webworker_matcher][handleMessage][${e.data.msg}][done][time][ms]`, new Date() - startTime);
  consoleLog(`[webworker_matcher][handleMessage][${e.data.msg}][done][time][ms]`, new Date() - startTime);

  if(postMessageFn) {
    
    startTime = new Date();
    postMessageFn();
    
    // eslint-disable-next-line no-console
    // console.log(`[webworker_matcher][postMessage][${e.data.msg}][done][ms]`, new Date() - startTime);
    consoleLog(`[webworker_matcher][postMessage][${e.data.msg}][done][ms]`, new Date() - startTime);
  }
  
}

