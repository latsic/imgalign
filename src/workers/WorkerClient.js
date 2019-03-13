
const singleton = Symbol();
const singletonEnforcer = Symbol();

const msgLoad = 'load';
const msgInit = 'init';
const msgParams = 'params';
const msgMatch = 'match';
const msgReset = 'reset';
const msgAlignedImage = 'alignedImage';
const msgBlendedImage = 'blendedImage';
const msgStitchedImage = 'stitchedImage';
const msgStitch = 'stitch';
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

const msgMultiStitchInit = 'multiStitchInit';
const msgMultiStitchReset = 'multiStitchReset';
const msgMultiStitchParams = 'multiStitchParams';
const msgMultiStitch = 'multiStitch';

const msgMultiStitchStart = 'multiStitchStart';
const msgMultiStitchNext = 'multiStitchNext';

export class WorkerClient {

  constructor(enforcer) {

    if(enforcer != singletonEnforcer) throw 'Cannot construct Singleton!';
    
    this._init();
  }

  static get instance() {
    if(!this[singleton]) {
      this[singleton] = new WorkerClient(singletonEnforcer);
    }
    return this[singleton];
  }

  _init() {
    this.worker = new Worker('js/imgalign.worker.js');
    this.worker.onmessage = e => this._handleMessage(e);
    this.worker.onerror = e => this._handleError(e);
  }

  reInitWorker() {
    this.__messageDone = null;
    this.worker.terminate();
    if(this.__messageAbort) this.__messageAbort();
    this.__messageAbort = null; 
    this._init();
  }

  async postWorkerMessageAsync(msg) {

    return new Promise((resolve, reject) => {

      this.__messageDone = function(){ resolve(...arguments)};
      this.__messageAbort = function(){ reject('ignore')};
      this.__error = (error) => reject(error);
      this.worker.postMessage(msg);
    });
  }

  async resetWorkerDataAsync() {
    return await this.postWorkerMessageAsync({ msg: msgReset });
  }

  async resetMultiStitchWorkerDataAsync() {
    return await this.postWorkerMessageAsync({ msg: msgMultiStitchReset });
  }

  async loadAsync() {
    return await this.postWorkerMessageAsync({ msg: msgLoad });
  }

  async _initAsync(fixedImage, movingImage) {
    return await this.postWorkerMessageAsync({
      msg: msgInit,
      payload: {
        fixedImage,
        movingImage
      }
    });
  }

  async _multiStitchInitAsync(images) {
    return await this.postWorkerMessageAsync({
      msg: msgMultiStitchInit,
      payload: {
        images
      }
    });
  }

  async _matchAsync(fixedImagePolygonPts, movingImagePolygonPts) {
    return await this.postWorkerMessageAsync({
      msg: msgMatch,
      payload: {
        fixedImagePolygonPts,
        movingImagePolygonPts
      }
    });
  }

  async _setParams(paramsArr) {
    return await this.postWorkerMessageAsync({
      msg: msgParams,
      payload: paramsArr.filter(param => param.id != 301)
    });
  }

  async _multiStitchSetParams(paramsArr) {
    return await this.postWorkerMessageAsync({
      msg: msgMultiStitchParams,
      payload: paramsArr.filter(param => param.id != 301)
    });
  }

  async _multiStitch(fieldsOfView) {
    return await this.postWorkerMessageAsync({
      msg: msgMultiStitch,
      payload: {
        fieldsOfView
      }
    });
  }

  async _multiStitchStart(fieldsOfView) {
    return await this.postWorkerMessageAsync({
      msg: msgMultiStitchStart,
      payload: {
        fieldsOfView
      }
    });
  }

  async _compareAsync(imageType, detTypesArr, fixedImagePolygonPts, movingImagePolygonPts) {
    return await this.postWorkerMessageAsync({
      msg: msgDetCompareImage,
      payload: {
        imageType,
        detTypesArr,
        fixedImagePolygonPts,
        movingImagePolygonPts
      }
    });
  }

  async compare_requestImageAsync(
    fixedImage, movingImage, paramsArr, imageType, detTypesArr,
    fixedImagePolygonPts, movingImagePolygonPts) {

    await this._initAsync(fixedImage, movingImage);
    await this._setParams(paramsArr);
    return await this._compareAsync(
      imageType, detTypesArr, fixedImagePolygonPts, movingImagePolygonPts);
  }

  
  async stitchAsync(
    projectionType1, projectionType2,
    seamBlend, colorTransfer,
    fieldOfViewFixedImage, fieldOfViewMovingImage,
    calcRotationYaw2, calcRotationPitch2,
    yaw1, pitch1, yaw2, pitch2) {
    
    return await this.postWorkerMessageAsync({
      msg: msgStitch,
      payload: {
        projectionType1,
        projectionType2,
        seamBlend,
        colorTransfer,
        fieldOfViewFixedImage,
        fieldOfViewMovingImage,
        calcRotationYaw2,
        calcRotationPitch2,
        yaw1,
        pitch1,
        yaw2,
        pitch2
      }
    });
  }

  async matchAsync(
    fixedImage, movingImage, paramsArr, fixedImagePolygonPts, movingImagePolygonPts) {

    await this._initAsync(fixedImage, movingImage);
    await this._setParams(paramsArr);
    return await this._matchAsync(fixedImagePolygonPts, movingImagePolygonPts);
  }

  async multiStitch(images, fieldsOfView, paramsArr) {
    await this._multiStitchInitAsync(images);
    await this._multiStitchSetParams(paramsArr);
    return await this._multiStitch(fieldsOfView);
  }

  async multiStitchRecompute(fieldsOfView, paramsArr) {
    await this._multiStitchSetParams(paramsArr);
    return await this._multiStitch(fieldsOfView);
  }

  async multiStitchStart(images, fieldsOfView, paramsArr) {
    await this._multiStitchInitAsync(images);
    await this._multiStitchSetParams(paramsArr);
    return await this._multiStitchStart(fieldsOfView);
  }

  async multiStitchNext() {
    return await this.postWorkerMessageAsync({
      msg: msgMultiStitchNext
    });
  }

  async multiStitchStartRecompute(fieldsOfView, paramsArr) {
    await this._multiStitchSetParams(paramsArr);
    return await this._multiStitchStart(fieldsOfView);
  }

  async requestAlignedImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgAlignedImage });
  }

  async requestBlendedImageAsync(weight, doOverlay) {
    return await this.postWorkerMessageAsync({
      msg: msgBlendedImage,
      payload: {
        weight,
        doOverlay
      }
    });
  }

  async requestStitchedImageAsync(weight) {
    return await this.postWorkerMessageAsync({
      msg: msgStitchedImage,
      payload: { weight }
    });
  }
  async requestStitchedImageWithKeyPointsAsync(weight) {
    return await this.postWorkerMessageAsync({
      msg: msgStitchedImageWithKeyPoints,
      payload: { weight }
    });
  }

  async requestBlendedImagePolygonAsync(weight, doOverlay, polygon, isFixedImage) {
    return await this.postWorkerMessageAsync({
      msg: isFixedImage
        ? msgBlendedImagePolygonFixedImage
        : msgBlendedImagePolygonMovingImage,
      payload: {
        weight,
        doOverlay,
        polygon,
        isFixedImage
      }
    });
  }

  async requestBlendedImageFloodFillAsync(weight, doOverlay, pts, floodFillTolerances, isFixedImage) {
    return await this.postWorkerMessageAsync({
      msg: isFixedImage
        ? msgBlendedImageFloodFillFixedImage
        : msgBlendedImageFloodFillMovingImage,
      payload: {
        weight,
        doOverlay,
        pts,
        floodFillTolerances,
        isFixedImage
      }
    });
  }

  async requestSideBySideImageGoodAsync() {
    return await this.postWorkerMessageAsync({ msg: msgSideBySideImageGood });
  }

  async requestSideBySideImageGoodInliersAsync() {
    return await this.postWorkerMessageAsync({ msg: msgSideBySideImageGoodInliers });
  }

  async requestKeyPointsGoodFixedImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgKeyPointsFixedImageGood });
  }

  async requestKeyPointsGoodMovingImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgKeyPointsMovingImageGood });
  }

  async requestKeyPointsInlierFixedImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgKeyPointsFixedImageInlier });
  }

  async requestKeyPointsInlierMovingImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgKeyPointsMovingImageInlier });
  }

  async requestResizedImageAsync(image, width, height) {
    return await this.postWorkerMessageAsync({
      msg: msgResize,
      payload: {
        image,
        width,
        height
      }
    });
  }

  _messageDone() {
    if(!this.__messageDone) return;
    this.__messageDone(...arguments);
    this.__messageDone = null;
  }

  _getImageData(data) {

    if(data.width == 0 || data.height == 0) {
      return null;
    }

    const clampedArray = new Uint8ClampedArray(data.buf, data.byteOffset, data.bufLen);
    return new ImageData(clampedArray, data.width, data.height);
  }

  _messageImage(rawImageData) {
    this._messageDone(this._getImageData(rawImageData));
  }

  _messageCompareImage(payload) {
    this._messageDone({
      imageData: this._getImageData(payload),
      timeUsedArr: payload.additionalData.timeUsedArr,
      keyPointsCountArr: payload.additionalData.keyPointsCountArr
    });
  }

  _messageStitchedImage(payload) {
    this._messageDone({
      imageData: this._getImageData(payload),
      fieldOfView: payload.additionalData
    });
  }

  _messageMultiStitchImage(payload) {
    this._messageDone({
      imageData: this._getImageData(payload),
      imageDataSmall: payload.imageDataSmall ? this._getImageData(payload.imageDataSmall) : null,
      stitchedImagesN: payload.additionalData
    });
  }

  _messageMultiStitchImageStart(payload) {
    this._messageDone({
      imageData: this._getImageData(payload),
      stitchIndices: payload.additionalData
    });
  }

  _handleMessage(event) {

    if(this._handleLogMessages(event.data)) return;

    if(event.data.info != 'done') {
      throw new Error(`Unknown message info: ${event.data.info}`);
    }

    switch(event.data.msg) {
      case msgLoad:
      case msgInit:
      case msgParams:
      case msgMatch:
      case msgReset:
      case msgMultiStitchInit:
      case msgMultiStitchReset:
      case msgMultiStitchParams:    
        this._messageDone(event.data.payload);
        break;
      case msgAlignedImage:
      case msgBlendedImage:
      case msgStitchedImage:
      case msgStitchedImageWithKeyPoints:
      case msgBlendedImagePolygonFixedImage:
      case msgBlendedImagePolygonMovingImage:
      case msgBlendedImageFloodFillFixedImage:
      case msgBlendedImageFloodFillMovingImage:
      case msgSideBySideImageGood:
      case msgSideBySideImageGoodInliers:
      case msgKeyPointsFixedImageGood:
      case msgKeyPointsMovingImageGood:
      case msgKeyPointsFixedImageInlier:
      case msgKeyPointsMovingImageInlier:
      case msgResize:
        this._messageImage(event.data.payload);
        break;
      case msgMultiStitchStart:
        this._messageMultiStitchImageStart(event.data.payload);
        break;
      case msgDetCompareImage:
        this._messageCompareImage(event.data.payload);
        break;
      case msgStitch:
        this._messageStitchedImage(event.data.payload);
        break;
      case msgMultiStitch:
      case msgMultiStitchNext:
        this._messageMultiStitchImage(event.data.payload);
        break;
      default:
    }
    // eslint-disable-next-line no-console
    console.log(`[WorkerClient][_handleMessage][${event.data.msg}][success]`);
  }

  _handleError(error) {
    
    if(this.__error) {
      this.__error(error);
      this.__error = null;
    }
    else {
      const line = 'Line: ' + (error.lineno ? error.lineno : 'unknown' + ' ');
      const file = 'File: ' + (error.filename ? error.filename : 'unknown' + ' ');
      const msg = 'Message: ' + (error.message ? error.message : 'unknown');

      // eslint-disable-next-line no-console
      console.log('[WorkerClient][_handleError]: ' + line + file + msg);
    }
  }

  _handleLogMessages({ msg, info, payload }) {

    function getArgs(info, payload) {
      if(info == 'extern') {
        return ['opencv', ...payload];
      }
      return payload;
    }

    switch(msg) {
      case msgConsoleLog:
        // eslint-disable-next-line no-console
        console.log(...getArgs(info, payload));
        return true;
      case msgConsoleInfo:
        // eslint-disable-next-line no-console
        console.info(...getArgs(info, payload));
        return true;
      case msgConsoleError:
        // eslint-disable-next-line no-console
        console.error(...getArgs(info, payload));
        return true;
      case msgConsoleAssert:
        // eslint-disable-next-line no-console
        console.assert(...getArgs(info, payload));
        return true;
      default: return false;
    }
  }
}