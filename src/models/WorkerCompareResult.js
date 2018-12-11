import { WorkerResult } from '@/models/WorkerResult'; 

export class WorkerCompareResult extends WorkerResult {

  constructor(name, imageDataToUrlFn) {
    super(name, imageDataToUrlFn);

    this._compareTimeUsedPerDet = [];
    this._compareKeyPointsCountPerDet = [];
  }

  get compareTimeUsedPerDet() {
    return this._compareTimeUsedPerDet;
  }
  
  get compareKeyPointsCountPerDet() {
    return this._compareKeyPointsCountPerDet;
  }
  set compareTimeUsedPerDet(value) {
    this._compareTimeUsedPerDet = value;
  }
  
  set compareKeyPointsCountPerDet(value) {
    this._compareKeyPointsCountPerDet = value;
  }
}