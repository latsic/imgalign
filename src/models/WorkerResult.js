import { Settings } from "@/models/Settings";

export class WorkerResult {
  constructor(name, imageDataToUrlFn) {
    
    this._imageData = null;
    this._imageDataUrl = null;
    this._success = null;
    this._time = 0;
    this._name = name;
    this._settings = new Settings();

    if(  imageDataToUrlFn == null
      || !typeof imageDataToUrlFn == 'function') {
      throw new Error('Need a valid conversion function')
    }
    this._imageDataToUrlFn = imageDataToUrlFn;
  }
  get name() {
    return this.name;
  }
  get imageData() {
    return this._imageData;
  }
  get imageDataUrl() {
    return this._imageDataUrl;
  }
  get imageDataValid() {
    return this._imageData
        && this._imageData.width > 0
        && this._imageData.height > 0;
  }
  get success() {
    return this._success;
  }
  get time() {
    return this._time;
  }
  get settings() {
    return this._settings;
  }
  set imageData(imageData) {
    this._imageData = imageData;
    if(imageData) {
      this._imageDataUrl = this._imageDataToUrlFn(imageData);
    }
    else {
      this._imageDataUrl = null;
    }
  }
  set success(success) {
    this._success = success;
  }
  set time(time) {
    this._time = time;
  }
  set settings(value) {
    this._settings = value;
  }

  setImageData(imageData, imageDataSmall) {
    this._imageData = imageData;
    if(imageData) {
      if(imageDataSmall) {
        this._imageDataUrl = this._imageDataToUrlFn(imageDataSmall);
      }
      else {
        this._imageDataUrl = this._imageDataToUrlFn(imageData);
      }
    }
    else {
      this._imageDataUrl = null;
    }
  }

}