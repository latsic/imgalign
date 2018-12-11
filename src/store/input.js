

import { ImageDataConversion } from '@/utilities/ImageDataConversion';
import { fixedImageName, movingImageName } from '@/models/constants/images';
import { matchName, compareName } from '@/models/constants/images';
import { close_snapDist } from '@/models/constants/polygon';

import defaultImage1 from '@/assets/bild1.jpg';
import defaultImage2 from '@/assets/bild2.jpg';
import { Polygon } from '@/models/Polygon';
import { paramTypes } from '@/models/constants/params';

const state = {

  imageData: {
    [fixedImageName]: null,
    [movingImageName]: null
  },
  imageDataUrls: {
    [fixedImageName]: null,
    [movingImageName]: null
  },
  polygons: {
    [fixedImageName]: new Polygon(),
    [movingImageName]: new Polygon()
  },
  floodFillTolerances: {
    [fixedImageName]: [],
    [movingImageName]: []
  }
}

const getters = {
  
  imageData(state){
    return name => state.imageData[name];
  },
  imageDataUrl(state, getters) {
    return name => getters['imageDataValid'](name)
      ? state.imageDataUrls[name]
      : null;
  },
  imageDataValid(state) {
    return name => state.imageData[name]
        && state.imageData[name].width > 0
        && state.imageData[name].height > 0;
  },
  allImageDataValid(state, getters) {
    return getters['imageDataValid'](fixedImageName)
        && getters['imageDataValid'](movingImageName)
  },
  allImageNames() {
    return [fixedImageName, movingImageName];
  },
  polygons(state) {
    return state.polygons;
  },
  polygon(state) {
    return name => state.polygons[name];
  },
  polygonClosedPts(state) {
    return name => {
      const polygon = state.polygons[name];
      if(!polygon.isClosed()) return []
      return polygon.pts;
    }
  },
  polygonClosedPtsFixedImage(state) {
    return state.polygons[fixedImageName].isClosed()
      ? state.polygons[fixedImageName].pts
      : [];
  },
  polygonClosedPtsMovingImage(state) {
    return state.polygons[movingImageName].isClosed()
      ? state.polygons[movingImageName].pts
      : [];
  },

  pt(state) {
    return name => {
      const ptsN = state.polygons[name].pts.length;
      return ptsN == 0 || state.polygons[name].isClosed()
        ? null
        : state.polygons[name].pts[ptsN - 1];
    }
  },
  ptFixedImage(state) {
    const ptsN = state.polygons[fixedImageName].pts.length;
    return ptsN == 0 || state.polygons[fixedImageName].isClosed()
      ? null
      : state.polygons[fixedImageName].pts[ptsN - 1];
  },
  ptMovingImage(state) {
    const ptsN = state.polygons[movingImageName].pts.length;
    return ptsN == 0 || state.polygons[movingImageName].isClosed()
      ? null
      : state.polygons[movingImageName].pts[ptsN - 1];
  },

  pathPts(state) {
    return name => {
      if(state.polygons[name].pts.length == 0) {
        return [];
      }
      if(state.polygons[name].isClosed()) {
        return state.polygons[name].pts.slice(1);
      }
      return state.polygons[name].pts;
    }
  },
  pathPtsFixedImage(state) {
    const poly = state.polygons[fixedImageName];
    if(poly.pts.length == 0) return [];
    if(poly.isClosed()) return poly.pts.slice(1);
    return poly.pts;
  },
  pathPtsMovingImage(state) {
    const poly = state.polygons[movingImageName];
    if(poly.pts.length == 0) return [];
    if(poly.isClosed()) return poly.pts.slice(1);
    return poly.pts;
  },

  floodFillTolerances(state) {
    return name => {
      if(state.floodFillTolerances[name].length == 0) return [];
      if(state.polygons[name].isClosed()) {
        return state.floodFillTolerances[name].slice(1);
      }
      return state.floodFillTolerances[name];
    }
  }

}

const mutations = {
 
  imageData(state, { name, imageData }) {
    state.imageData[name] = imageData;
  },
  _imageDataUrl(state, { name, imageDataUrl }) {
    state.imageDataUrls[name] = imageDataUrl;
  },
  addPolygonPt(state, { name, pt, floodFillTolerance }) {

    if(state.polygons[name].isClosed()) {
      state.polygons[name] = new Polygon();
      state.floodFillTolerances[name] = [];
    }
    else {
      state.polygons[name].addPtAndCloseIf(pt, close_snapDist);
      state.floodFillTolerances[name].push(floodFillTolerance || floodFillTolerance === 0 ? floodFillTolerance : 10);
    }
  },
  resetPolygon(state, name) {
    state.polygons[name] = new Polygon();
    state.floodFillTolerances[name] = [];
  }
}

const actions = {
  init(context) {

    const defaultImages = [
      { name: fixedImageName, dataUrl: defaultImage1 },
      { name: movingImageName, dataUrl: defaultImage2 }
    ];

    for(const { name, dataUrl } of defaultImages) {
      if(!context.getters['imageDataValid'](name)) {
        const img = new Image();
        img.onload = () => {
          const imageData = ImageDataConversion.imageDataFromImageSrc(img);
          context.dispatch('imageData', { name, imageData });
          img.onload = null;
        }
        img.src = dataUrl;
      }
    }
  },
  async imageData({ commit, dispatch, rootGetters }, { name, imageData }) {
    
    if(imageData && rootGetters['worker/ready']) {
      const maxPixelsN = rootGetters['settings/param'](paramTypes.imageCapInput.id);
      if(imageData.width * imageData.height > maxPixelsN) {
        const scaleF = Math.sqrt(maxPixelsN / (imageData.width * imageData.height));
        const height = Math.floor(imageData.height * scaleF);
        const width = Math.floor(imageData.width * scaleF);
        if(rootGetters['worker/ready'])
        await dispatch('worker/setInputImageResized', {
          imageName: name,
          imageDataSrc: imageData,
          width,
          height }, {
            root: true
            });
        return;
      }
    }

    commit('imageData', { name, imageData });
    
    dispatch('_imageDataUrl', {name, imageData });
    commit('resetPolygon', name);
    
    
    if(rootGetters['worker/ready']) {
      dispatch('worker/resetWorkerData', null, { root: true });
      commit('worker/results/imageData', { name: matchName, imageData: null}, { root: true });
      commit('worker/results/imageData', { name: compareName, imageData: null}, { root: true });
    }
  },
  _imageDataUrl({ commit }, { name, imageData }) {
    if(!imageData) commit('_imageDataUrl', { name, imageDataUrl: null });
    else commit('_imageDataUrl', { name, imageDataUrl: ImageDataConversion.imageSrcFromImageData(imageData) });
  },
  imageFile(context, { name, file }) {

    const img = new Image(); 
    img.onload = () => {
      const imageData = ImageDataConversion.imageDataFromImageSrc(img);
      context.dispatch('imageData', { name, imageData });
      img.onload = null;
    }
    img.src = URL.createObjectURL(file)
  },
  imageUrlBase64(context, { name, url }) {
    const img = new Image(); 
    img.onload = () => {
      const imageData = ImageDataConversion.imageDataFromImageSrc(img);
      context.dispatch('imageData', { name, imageData });
      img.onload = null;
    }
    img.src = url;
  }
}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions
};