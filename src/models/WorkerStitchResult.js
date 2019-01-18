
import { WorkerResult } from '@/models/WorkerResult'; 

export class WorkerStitchResult extends WorkerResult {

  constructor(name, imageDataToUrlFn) {
    super(name, imageDataToUrlFn);

    this._projected = false;
    this._fieldOfView = null;
  }

  get projected() {
    return this._projected;
  }
  set projected(value) {
    this._projected = value;
  }
  
  get fieldOfView() {
    return this._fieldOfView;
  }
  set fieldOfView(value) {
    this._fieldOfView = value; 
  }
}