
import { WorkerMatchResult } from '@/models/WorkerMatchResult';
import { WorkerCompareResult } from '@/models/WorkerCompareResult';

import { matchName, compareName } from '@/models/constants/images';
import { ImageDataConversion } from '@/utilities/ImageDataConversion';

const state = {
  imageResults: {
    [matchName]: new WorkerMatchResult(matchName, ImageDataConversion.imageSrcFromImageData),
    [compareName]: new WorkerCompareResult(compareName, ImageDataConversion.imageSrcFromImageData)
  }
}

const getters = {
  imageData(state) {
    return name => state.imageResults[name].imageData;
  },
  imageDataUrl(state) {
    return name => state.imageResults[name].imageDataUrl;
  },
  imageDataValid(state) {
    return name => state.imageResults[name].imageDataValid;
  },
  success(state) {
    return name => state.imageResults[name].success;
  },
  time(state) {
    return name => state.imageResults[name].time;
  },
  settings(state) {
    return name => state.imageResults[name].settings;
  },

  matcherImageType(state) {
    return state.imageResults[matchName].matcherImageType;
  },
  matcherImageBlend(state) {
    return state.imageResults[matchName].matcherImageBlend;
  },

  compareTimeUsedPerDet(state) {
    return state.imageResults[compareName].compareTimeUsedPerDet;
  },
  compareKeyPointsCountPerDet(state) {
    return state.imageResults[compareName].compareKeyPointsCountPerDet;
  }
}

const mutations = {
  imageData(state, { name, imageData }) {
    state.imageResults[name].imageData = imageData;
  },
  success(state, { name, success }) {
    state.imageResults[name].success = success;
  },
  time(state, { name, time }) {
    state.imageResults[name].time = time;
  },
  settings(state, { name, settings }) {
    if(!settings) return;
    settings.copyValuesTo(state.imageResults[name].settings);
  },

  matcherImageType(state, value) {
    state.imageResults[matchName].matcherImageType = value;
  },
  matcherImageTypeSetDefault(state) {
    state.imageResults[matchName].setDefault();
  },
  matcherImageBlend(state, value) {
    state.imageResults[matchName].matcherImageBlend = value;
  },

  compareTimeUsedPerDet(state, valueArr) {
    state.imageResults[compareName].compareTimeUsedPerDet = valueArr;
  },
  compareKeyPointsCountPerDet(state, valueArr) {
    state.imageResults[compareName].compareKeyPointsCountPerDet = valueArr;
  }
}

const actions = {
}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions
};