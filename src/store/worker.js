
import { WorkerClient } from '@/workers/WorkerClient';
import { matchName, compareName } from '@/models/constants/images';
import { fixedImageName, movingImageName } from '@/models/constants/images';
import { paramTypes } from '@/models/constants/params';
import { matcherImageType } from '@/models/constants/matcherImageType';
import results from '@/store/results';

const state = {
  loadStartTime: null,
  loadTime: null,
  ready: false,
  error: null,
  busy: false,
  busyImage: false
}

const getters = {
  ready(state) {
    return state.ready;
  },
  loadTime(state) {
    return state.loadTime;
  },
  error(state) {
    return state.error;
  },
  busy(state) {
    return state.busy;
  }
}

const mutations = {
  _init(state) {
    if(state.ready) return;
    state.loadStartTime = new Date();
  },
  ready(state, value) {

    if(value && state.loadStartTime) {
      state.loadTime = new Date() - state.loadStartTime;
    }
    state.ready = value;
    // eslint-disable-next-line no-console
    console.log(`[store][worker][mutation][ready][time][ms]: ${state.loadTime}, ${value}`);
  },
  error(state, error) {
    state.error = error;
  },
  busy(state, value) {
    state.busy = value;
  },
  busyImage(state, value) {
    state.busyImage = value;
  }
}

const actions = {
  async load(context) {
    if(context.getters['ready']) return;
    if(context.getters['busy']) return;

    context.commit('busy', true);
    context.commit('_init', context);
    try {
      await WorkerClient.instance.loadAsync();
      context.dispatch('ready', true);
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][load][Error]', error);
      context.commit('error', error);
    }
    finally {
      context.commit('busy', false);
    }
  },
  error(context, error) {
    context.commit('error', error);
  },
  async resetWorkerData() {
    await WorkerClient.instance.resetWorkerDataAsync();
  },
  ready(context, value) {
    context.commit('ready', value);
  },
  busy(context, value) {
    context.commit('busy', value);
  },

  async _requestImage(context, fn) {
    try {

      if(context.state.busy) {
        throw new Error('Cannot request image while busy');
      }
      if(context.state.busyImage) {
        throw new Error('Cannot request image while already busy getting an image');
      }
      context.commit('busyImage', true);

      context.commit('error', null);
      const imageData = await fn();
      context.commit('results/imageData', { name: matchName, imageData });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][generic][Error]', error);
      context.commit('error', error);
      context.commit('results/imageData', { name: matchName, imageData: null });
      context.commit('results/success', { name: matchName, success: 0 });
    }
    finally{
      context.commit('busyImage', false);
    }
  },
  async computeAlignedImage(context, { fixedImage, movingImage, fixedImagePolygonPts, movingImagePolygonPts, settings }) {
    
    try {

      if(context.state.busyImage) {
        throw new Error('Cannot compute image while already busy getting an image');
      }

      context.commit('busy', true);
      context.commit('error', null);
      const matchStartTime = new Date();

      const success = await WorkerClient.instance.matchAsync(
        fixedImage, movingImage, settings.getIdValueArrExcludingDefaults(),
        fixedImagePolygonPts, movingImagePolygonPts);

      context.commit('results/time', { name: matchName, time: new Date() - matchStartTime });
      context.commit('results/success', { name: matchName, success });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][computeAlignedImage][Error]', error);
      context.commit('error', error);
      context.commit('results/imageData', { name: matchName, imageData: null });
      context.commit('results/success', { name: matchName, success: 0 });
    }
    finally {
      context.commit('busy', false);
    }

    await context.dispatch('requestImage');
  },
  async requestImage(context) {

    const matcherImageTypeValue = context.getters['results/matcherImageType'];

    switch(matcherImageTypeValue) {
      case matcherImageType.aligned:
        await context.dispatch('requestAlignedImage');
        break;
      case matcherImageType.blended:
        await context.dispatch('requestBlendedImage',
          context.getters['results/matcherImageBlend']);
        break;

      case matcherImageType.selectionImage1:
        if(context.rootGetters['input/polygonClosedPts'](fixedImageName).length !== 0) {
          await context.dispatch('requestBlendedImagePolygon', {
            weight: context.getters['results/matcherImageBlend'],
            doOverlay: context.rootGetters['settings/param'](paramTypes.alignSelectionOverlay.id),
            polygon: context.rootGetters['input/polygonClosedPts'](fixedImageName),
            isFixedImage: true });
        }
        else {
          await context.dispatch('requestBlendedImageFloodFill', {
            weight: context.getters['results/matcherImageBlend'],
            doOverlay: context.rootGetters['settings/param'](paramTypes.alignSelectionOverlay.id),
            pts: context.rootGetters['input/pathPts'](fixedImageName),
            floodFillTolerances: context.rootGetters['input/floodFillTolerances'](fixedImageName),
            isFixedImage: true });
        }
        break;
      case matcherImageType.selectionImage2:
        if(context.rootGetters['input/polygonClosedPts'](movingImageName).length !== 0) {
          await context.dispatch('requestBlendedImagePolygon', {
            weight: context.getters['results/matcherImageBlend'],
            doOverlay: context.rootGetters['settings/param'](paramTypes.alignSelectionOverlay.id),
            polygon: context.rootGetters['input/polygonClosedPts'](movingImageName),
            isFixedImage: false });
        }
        else {
          await context.dispatch('requestBlendedImageFloodFill', {
            weight: context.getters['results/matcherImageBlend'],
            doOverlay: context.rootGetters['settings/param'](paramTypes.alignSelectionOverlay.id),
            pts: context.rootGetters['input/pathPts'](movingImageName),
            floodFillTolerances: context.rootGetters['input/floodFillTolerances'](movingImageName),
            isFixedImage: false });
        }
        break;

      case matcherImageType.keyPointsGoodFixed:
        await context.dispatch('requestKeyPointsGoodFixedImage');
        break;
      case matcherImageType.keyPointsGoodMoving:
        await context.dispatch('requestKeyPointsGoodMovingImage');
        break;
      case matcherImageType.keyPointsInlierFixed:
        await context.dispatch('requestKeyPointsInlierFixedImage');
        break;
      case matcherImageType.keyPointsInlierMoving:
        await context.dispatch('requestKeyPointsInlierMovingImage');
        break;
      case matcherImageType.sideBySideGood:
        await context.dispatch('requestSideBySideImageGood');
        break;
      case matcherImageType.sideBySideGoodInliers:
        await context.dispatch('requestSideBySideImageGoodInliers');
        break;
      
      default: {break;}
    }
  },
  async requestAlignedImage(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestAlignedImageAsync();
    });
  },
  async requestBlendedImage(context, value = 0.5) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImageAsync(value);
    });
  },

  async requestBlendedImagePolygon(context, { weight, doOverlay, polygon, isFixedImage }) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImagePolygonAsync(
        weight, doOverlay, polygon, isFixedImage);
    });
  },

  async requestBlendedImageFloodFill(context, { weight, doOverlay, pts, floodFillTolerances, isFixedImage }) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImageFloodFillAsync(
        weight, doOverlay, pts, floodFillTolerances, isFixedImage);
    });
  },

  async requestSideBySideImageGood(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestSideBySideImageGoodAsync();
    });
  },
  async requestSideBySideImageGoodInliers(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestSideBySideImageGoodInliersAsync();
    });
  },
  async requestKeyPointsGoodFixedImage(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsGoodFixedImageAsync();
    });
  },
  async requestKeyPointsGoodMovingImage(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsGoodMovingImageAsync();
    });
  },
  async requestKeyPointsInlierFixedImage(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsInlierFixedImageAsync();
    });
  },
  async requestKeyPointsInlierMovingImage(context) {
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsInlierMovingImageAsync();
    });
  },
  async computeCompareImage(context, { 
    fixedImage, movingImage, fixedImagePolygonPts, movingImagePolygonPts, settings }) {
    try {
      context.commit('busy', true);
      context.commit('error', null);

      const detTypeValues = settings.getParamValuesById(paramTypes.compareDetTypes.id);
      const imageType = settings.getParamValueById(paramTypes.compareImageType.id);
      const paramsArr = settings.getIdValueArrExcludingDefaults();

      const compareStartTime = new Date();

      const { imageData, timeUsedArr, keyPointsCountArr } =
        await WorkerClient.instance.compare_requestImageAsync(
          fixedImage, movingImage, paramsArr, imageType, detTypeValues, fixedImagePolygonPts, movingImagePolygonPts);

      context.commit('results/imageData', { name: compareName, imageData});     
      context.commit('results/compareTimeUsedPerDet', timeUsedArr);
      context.commit('results/compareKeyPointsCountPerDet', keyPointsCountArr);
      context.commit('results/time', { name: compareName, time: new Date() - compareStartTime });
      context.commit('results/success', { name: compareName, success: 1 });
      context.commit('results/settings', { name: compareName, settings });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][generic][Error]', error);
      context.commit('error', error);
      context.commit('results/imageData', { name: compareName, imageData: null });
      context.commit('results/success', { name: compareName, success: 0 });
    }
    finally{
      context.commit('busy', false);
    }
  },

  async setInputImageResized(context, { imageName, imageDataSrc, width, height }) {

    try {

      const imageData = await WorkerClient.instance.requestResizedImageAsync(imageDataSrc, width, height);
      context.dispatch('input/imageData', { name: imageName, imageData }, { root: true });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][setInputImageResized][Error]', error);
      // context.commit('error', error);
    }
    finally {
      // context.commit('busyImage', false);
    }
  }

};
const modules = {
  results
}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions,
  modules
};
