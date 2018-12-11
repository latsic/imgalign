
const singleton = Symbol();
const singletonEnforcer = Symbol();

const msgLoad = 'load';
const msgInit = 'init';
const msgParams = 'params';
const msgMatch = 'match';
const msgReset = 'reset';
const msgAlignedImage = 'alignedImage';
const msgBlendedImage = 'blendedImage';
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

export class WorkerClient {

  constructor(enforcer) {

    if(enforcer != singletonEnforcer) throw 'Cannot construct Singleton!';
    
    this.worker = new Worker('js/imgalign.worker.js');
    this.worker.onmessage = e => this._handleMessage(e);
    this.worker.onerror = e => this._handleError(e);
  }

  static get instance() {
    if(!this[singleton]) {
      this[singleton] = new WorkerClient(singletonEnforcer);
    }
    return this[singleton];
  }

  async postWorkerMessageAsync(msg) {

    return new Promise((resolve, reject) => {

      this.__messageDone = function(){ resolve(...arguments)};
      this.__error = (error) => reject(error);
      this.worker.postMessage(msg);
    });
  }

  async resetWorkerDataAsync() {
    return await this.postWorkerMessageAsync({ msg: msgReset });
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

  async matchAsync(
    fixedImage, movingImage, paramsArr, fixedImagePolygonPts, movingImagePolygonPts) {

    await this._initAsync(fixedImage, movingImage);
    await this._setParams(paramsArr);
    return await this._matchAsync(fixedImagePolygonPts, movingImagePolygonPts);
  }

  async requestAlignedImageAsync() {
    return await this.postWorkerMessageAsync({ msg: msgAlignedImage });
  }

  async requestBlendedImageAsync(value) {
    return await this.postWorkerMessageAsync({ msg: msgBlendedImage, payload: { value }});
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
    this.__messageDone(...arguments);
    this.__messageDone = null;
  }

  _messageImage(rawImageData) {
    
    const clampedArray = new Uint8ClampedArray(rawImageData.buf, rawImageData.byteOffset, rawImageData.bufLen);
    const imageData = new ImageData(clampedArray, rawImageData.width, rawImageData.height);
    this._messageDone(imageData);
  }

  _messageCompareImage(payload) {
    const clampedArray = new Uint8ClampedArray(payload.buf, payload.byteOffset, payload.bufLen);
    const imageData = new ImageData(clampedArray, payload.width, payload.height);
    this._messageDone({
      imageData,
      timeUsedArr: payload.additionalData.timeUsedArr,
      keyPointsCountArr: payload.additionalData.keyPointsCountArr });
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
        this._messageDone(event.data.payload);
        break;
      case msgAlignedImage:
      case msgBlendedImage:
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
      case msgDetCompareImage:
        this._messageCompareImage(event.data.payload);
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
        return ['extern', ...payload];
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