
import { WorkerResult } from '@/models/WorkerResult'; 
import { matcherImageType } from '@/models/constants/matcherImageType';

export class WorkerMatchResult extends WorkerResult {

  constructor(name, imageDataToUrlFn) {
    super(name, imageDataToUrlFn);

    this._matcherImageType = matcherImageType.aligned;
    this._matcherImageBlend = 0.5;
  }

  get matcherImageType() {
    return this._matcherImageType;
  }
  set matcherImageType(value) {
    this._matcherImageType = value;
  }
  
  get matcherImageBlend() {
    return this._matcherImageBlend;
  }
  set matcherImageBlend(value) {
    this._matcherImageBlend = value; 
  }
  
  setDefault() {
    this._matcherImageType = matcherImageType.aligned;
  }
}