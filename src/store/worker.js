
import { WorkerClient } from '@/workers/WorkerClient';
import { matchName, compareName, stitchName, multiStitchName } from '@/models/constants/images';
import { fixedImageName, movingImageName } from '@/models/constants/images';
import { paramTypes } from '@/models/constants/params';
import { matcherImageType } from '@/models/constants/matcherImageType';
import results from '@/store/results';

const state = {
  loadStartTime: null,
  loadTime: null,
  ready: false,
  error: null,
  busyImage: false,
  busyCompute: false,
  busyLoad: false,
  currentActionInfo: '',
  lastCompletedActionTime: 0,
  currentActionStartTime: null
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
    return state.busyImage || state.busyCompute || state.busyLoad;
  },
  busyImage(state) {
    return state.busyImage;
  },
  busyCompute(state) {
    return state.busyCompute;
  },
  busyLoad(state) {
    return state.busyLoad;
  },
  currentActionInfo(state) {
    return state.currentActionInfo;
  },
  currentActionTime(state) {
    if(state.currentActionInfo && state.currentActionStartTime) {
      return new Date() - state.currentActionStartTime;
    }
    return null;
  },
  lastCompletedActionTime(state) {
    return state.lastCompletedActionTime;
  }
}

const mutations = {
  _init(state) {
    if(state.ready) return;
    state.loadStartTime = new Date();
  },
  _ready(state, value) {

    if(value && state.loadStartTime) {
      state.loadTime = new Date() - state.loadStartTime;
    }
    state.ready = value;
    // eslint-disable-next-line no-console
    console.log(`[store][worker][mutation][ready][time][ms]: ${state.loadTime}, ${value}`);
  },
  error(state, error) {
    //if(error != 'ignore') {
      state.error = error;
    //}
  },
  _busyCompute(state, value) {
    state.busyCompute = value;
  },
  _busyImage(state, value) {
    state.busyImage = value;
  },
  _busyLoad(state, value) {
    state.busyLoad = value;
  },
  currentActionInfo(state, value) {
    state.currentActionInfo = value;
    if(value) {
      state.currentActionStartTime = new Date();
    }
    else if(state.currentActionStartTime) {
      state.lastCompletedActionTime = new Date() - state.currentActionStartTime;
      state.currentActionStartTime = null;
    }
  }
}

const actions = {
  async load(context) {
    if(context.getters['ready']) return;
    if(context.getters['busy']) return;

    context.commit('_busyLoad', true);
    context.commit('_init', context);
    context.commit('currentActionInfo', 'Loading opencv');
    try {
      await WorkerClient.instance.loadAsync();
      context.commit('_ready', true);
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][load][Error]', error);
      context.commit('error', error);
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyLoad', false);
    }
  },
  async reload(context) {
    WorkerClient.instance.reInitWorker();
    context.commit('error', null);
    context.commit('_busyCompute', false);
    context.commit('_ready', false);
    await context.dispatch('load');
  },
  error(context, error) {
    context.commit('error', error);
  },
  async resetWorkerData(context) {

    if(context.getters['busy']) return;

    await WorkerClient.instance.resetWorkerDataAsync();
    context.commit('results/success', { name: matchName, success: 0 });
    context.commit('error', null);
  },

  async multiStitchResetWorkerData(context) {

    if(context.getters['busy']) return;

    await WorkerClient.instance.resetMultiStitchWorkerDataAsync();
    context.commit('results/success', { name: multiStitchName, success: 0 });
    context.commit('error', null);
  },

  async _requestImage(context, fn) {
    
    if(context.getters['busy']) {
      throw new Error('Cannot request image while busy');
    }
    if(!context.getters['results/success'](matchName)) {
      //throw new Error('Cannot request an image without a successfull match result!');
      return;
    }

    try {
      context.commit('_busyImage', true);
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
      context.commit('currentActionInfo', '');
      context.commit('_busyImage', false);
    }
  },

  async computeMultiStitchedImage(context, { images, fieldsOfView, settings }) {
    if(context.getters['busy']) {
      return;
    }
    try {
      context.commit('_busyCompute', true);
      context.commit('error', null);
      context.commit('currentActionInfo', 'Stitching');
      const stitchingStartTime = new Date();

      const { imageData, stitchedImagesN } = await WorkerClient.instance.multiStitch(
            images, fieldsOfView, settings.getIdValueArrExcludingDefaults());
      // const { imageData, stitchedImagesN } = !context.getters['results/imageDataValid'](multiStitchName)
      //   ? await WorkerClient.instance.multiStitch(
      //       images, fieldsOfView, settings.getIdValueArrExcludingDefaults())
      //   : await WorkerClient.instance.multiStitchRecompute(
      //       fieldsOfView, settings.getIdValueArrExcludingDefaults());
      
      context.commit('results/imageData', { name: multiStitchName, imageData});   
      context.commit('results/time', { name: multiStitchName, time: new Date() - stitchingStartTime });
      context.commit('results/success', { name: multiStitchName, success: stitchedImagesN == images.length ? 1 : 0 });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][computeStitchedImage][Error]', error);
      context.commit('error', error);
      context.commit('results/imageData', { name: multiStitchName, imageData: null });
      context.commit('results/success', { name: multiStitchName, success: 0 });
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyCompute', false);
    }
  },

  async computeMultiStitchImageInSteps(context, { images, fieldsOfView, settings }) {
    
    if(context.getters['busy']) {
      return;
    }
    try {
      context.commit('_busyCompute', true);
      context.commit('error', null);
      context.commit('currentActionInfo', 'Detecting Features');
      const stitchingStartTime = new Date();

      const { imageData, stitchIndices } = await WorkerClient.instance.multiStitchStart(
        images, fieldsOfView, settings.getIdValueArrExcludingDefaults());
      
      for(let i = 0; i < images.length; ++i) {
        if(!stitchIndices.some(imageIndex => imageIndex == i)) {
          // eslint-disable-next-line no-console
          console.warn('[store][worker][computeMultiStitchImageInSteps]',
            `No match found for image with index ${i}`);
        }
      }

      context.commit('results/imageData', { name: multiStitchName, imageData });
      
      let imagesRemainingN = stitchIndices.length - 1;
      while(imagesRemainingN > 0) {

        //context.commit('currentActionInfo', `Stitching, ${imagesRemainingN} images remaining`);

        const { imageData, stitchedImagesN } = await WorkerClient.instance.multiStitchNext();
        if(stitchedImagesN == 0) {
          imagesRemainingN = 0;
          // eslint-disable-next-line no-console
          console.warn('[store][worker][computeMultiStitchImageInSteps]',
            'Unexpected result from multiStitchNext');
        }
        else {
          imagesRemainingN = stitchIndices.length - stitchedImagesN;
          context.commit('results/imageData', { name: multiStitchName, imageData});
        }
      }
      context.commit('results/time', { name: multiStitchName, time: new Date() - stitchingStartTime });
      context.commit('results/success', { name: multiStitchName, success: 1 });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][computeMultiStitchImageInSteps][Error]', error);
      context.commit('error', error);
      // context.commit('results/imageData', { name: multiStitchName, imageData: null });
      context.commit('results/success', { name: multiStitchName, success: 0 });
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyCompute', false);
    }
  },

  async computeAlignedImage(context, { fixedImage, movingImage, fixedImagePolygonPts, movingImagePolygonPts, settings }) {
    
    if(context.getters['busy']) {
      return;
      // throw new Error('Already busy with another task');
    }

    try {

      context.commit('_busyCompute', true);
      context.commit('error', null);
      context.commit('currentActionInfo', 'Feature detection');
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
      context.commit('currentActionInfo', '');
      context.commit('_busyCompute', false);
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
        await context.dispatch('requestBlendedImage', {
          weight: context.getters['results/matcherImageBlend'], 
          doOverlay: context.rootGetters['settings/param'](paramTypes.alignSelectionOverlay.id) });
        break;

      case matcherImageType.stitched:
        await context.dispatch('requestStitchedImage', {
          weight: context.getters['results/matcherImageBlend'] }); 
        break;
      case matcherImageType.stitchedWithKeyPoints:
        await context.dispatch('requestStitchedImageWithKeyPoints', {
          weight: context.getters['results/matcherImageBlend'] }); 
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
      
      default: { break; }
    }
  },
  async requestAlignedImage(context) {
    context.commit('currentActionInfo', 'Requesting aligned image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestAlignedImageAsync();
    });
  },
  async requestBlendedImage(context, { weight, doOverlay }) {
    context.commit('currentActionInfo', 'Requesting blended image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImageAsync(
        weight, doOverlay);
    });
  },
  async requestStitchedImage(context, { weight }) {
    context.commit('currentActionInfo', 'Requesting stitched image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestStitchedImageAsync(weight);
    });
  },
  async requestStitchedImageWithKeyPoints(context, { weight }) {
    context.commit('currentActionInfo', 'Requesting stitched image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestStitchedImageWithKeyPointsAsync(weight);
    });
  },

  async requestBlendedImagePolygon(context, { weight, doOverlay, polygon, isFixedImage }) {
    context.commit('currentActionInfo', 'Requesting blended image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImagePolygonAsync(
        weight, doOverlay, polygon, isFixedImage);
    });
  },

  async requestBlendedImageFloodFill(context, { weight, doOverlay, pts, floodFillTolerances, isFixedImage }) {
    context.commit('currentActionInfo', 'Requesting blended image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestBlendedImageFloodFillAsync(
        weight, doOverlay, pts, floodFillTolerances, isFixedImage);
    });
  },

  async requestSideBySideImageGood(context) {
    context.commit('currentActionInfo', 'Requesting side by side image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestSideBySideImageGoodAsync();
    });
  },
  async requestSideBySideImageGoodInliers(context) {
    context.commit('currentActionInfo', 'Requesting side by side image');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestSideBySideImageGoodInliersAsync();
    });
  },
  async requestKeyPointsGoodFixedImage(context) {
    context.commit('currentActionInfo', 'Requesting image with key points');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsGoodFixedImageAsync();
    });
  },
  async requestKeyPointsGoodMovingImage(context) {
    context.commit('currentActionInfo', 'Requesting image with key points');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsGoodMovingImageAsync();
    });
  },
  async requestKeyPointsInlierFixedImage(context) {
    context.commit('currentActionInfo', 'Requesting image with key points');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsInlierFixedImageAsync();
    });
  },
  async requestKeyPointsInlierMovingImage(context) {
    context.commit('currentActionInfo', 'Requesting image with key points');
    context.dispatch('_requestImage', async () => {
      return await WorkerClient.instance.requestKeyPointsInlierMovingImageAsync();
    });
  },
  async computeCompareImage(context, { 
    fixedImage, movingImage, fixedImagePolygonPts, movingImagePolygonPts, settings }) {
    
    if(context.getters['busy']) {
      // return;
      throw new Error('Already busy with another task');
    }
    
    try {
      context.commit('_busyCompute', true);
      context.commit('error', null);
      context.commit('currentActionInfo', 'Feature detection');

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
      context.commit('currentActionInfo', '');
      context.commit('_busyCompute', false);
    }
  },

  async displayStitchedImage(context)
  {
    if(context.getters['busy']) {
      return;
    }
    try {
      context.commit('_busyImage', true);
      context.commit('currentActionInfo', 'Stitching image');

      const projectionType1 = context.rootGetters['settings/param'](paramTypes.stitch_projection.id);
      const projectionType2 = context.rootGetters['settings/param'](paramTypes.stitch_projection2.id);
      const seamBlend = context.rootGetters['settings/param'](paramTypes.stitch_seamBlend.id);
      const colorTransfer = context.rootGetters['settings/param'](paramTypes.stitch_colorTransfer.id);
      const fieldOfView1 = context.rootGetters['settings/param'](paramTypes.stitch_fieldOfView1.id);
      const fieldOfView2 = context.rootGetters['settings/param'](paramTypes.stitch_fieldOfView2.id);
      const yaw1 = context.rootGetters['settings/param'](paramTypes.stitch_yaw1.id);
      const pitch1 = context.rootGetters['settings/param'](paramTypes.stitch_pitch1.id);
      const yaw2 = context.rootGetters['settings/param'](paramTypes.stitch_yaw2.id);
      const pitch2 = context.rootGetters['settings/param'](paramTypes.stitch_pitch2.id);
      const calcYaw2 = context.rootGetters['settings/param'](paramTypes.stitch_yaw2Auto.id);
      const calcPitch2 = context.rootGetters['settings/param'](paramTypes.stitch_pitch2Auto.id);

      const { fieldOfView, imageData } = await WorkerClient.instance.stitchAsync(
        projectionType1, projectionType2,
        seamBlend, colorTransfer,
        fieldOfView1, fieldOfView2,
        calcYaw2, calcPitch2,
        yaw1, pitch1, yaw2, pitch2);

      const isProjected = projectionType2 != paramTypes.stitch_projectionTypeNone;
      context.commit('results/stitcherFieldOfView', Math.round(fieldOfView));
      context.commit('results/stitcherProjected', isProjected);
      context.commit('results/imageData',  { name: stitchName, imageData}); 
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][displayStitchedImage][Error]', error);
      context.commit('results/imageData', { name: stitchName, imageData: null });
      context.commit('error', error);
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyImage', false);
    }
  },

  async computeStitchedImage(context, { 
    fixedImage, movingImage, fixedImagePolygonPts, movingImagePolygonPts, settings }) {
    
    if(context.getters['busy']) {
      return;
    }

    try {

      context.commit('_busyCompute', true);
      context.commit('error', null);
      context.commit('currentActionInfo', 'Feature detection');
      const stitchStartTime = new Date();

      let success = await WorkerClient.instance.matchAsync(
        fixedImage, movingImage, settings.getIdValueArrExcludingDefaults(),
        fixedImagePolygonPts, movingImagePolygonPts);
      
      context.commit('results/time', { name: stitchName, time: new Date() - stitchStartTime });
      context.commit('results/success', { name: stitchName, success });
    }
    catch(error) {

      // eslint-disable-next-line no-console
      console.log('[store][worker][computeStitchedImage][Error]', error);
      context.commit('error', error);
      context.commit('results/imageData', { name: stitchName, imageData: null });
      context.commit('results/success', { name: stitchName, success: 0 });
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyCompute', false);
    }

  },

  async setInputImageResized(context, { imageName, imageDataSrc, width, height }) {

    if(context.getters['busy']) {
      // return;
      throw new Error('Already busy with another task');
    }
    
    try {

      context.commit('_busyImage', true);
      context.commit('currentActionInfo', 'Resizing image');

      const imageData = await WorkerClient.instance.requestResizedImageAsync(imageDataSrc, width, height);
      context.dispatch('input/imageData', { name: imageName, imageData }, { root: true });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][setInputImageResized][Error]', error);
      context.commit('error', error);
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyImage', false);
    }
  },
  async setMultiInputImageResized(context, { imageDataSrc, width, height }) {

    if(context.getters['busy']) {
      // return;
      throw new Error('Already busy with another task');
    }
    
    try {

      context.commit('_busyImage', true);
      context.commit('currentActionInfo', 'Resizing image');

      const imageData = await WorkerClient.instance.requestResizedImageAsync(imageDataSrc, width, height);
      context.dispatch('multiInput/imageData', imageData, { root: true });
    }
    catch(error) {
      // eslint-disable-next-line no-console
      console.log('[store][worker][setMultiInputImageResized][Error]', error);
      context.commit('error', error);
    }
    finally {
      context.commit('currentActionInfo', '');
      context.commit('_busyImage', false);
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
