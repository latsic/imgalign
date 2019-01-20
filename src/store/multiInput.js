
import { ImageDataConversion } from '@/utilities/ImageDataConversion';
import { paramTypes } from '@/models/constants/params';
import { ExifHelper } from '@/utilities/ExifHelper';
import { multiStitchName } from '@/models/constants/images';

const state = {
  imageDataArray: [],
  imageDataUrlsArray: [],
  imageFieldOfViewArray: [],
  imageFieldOfViewInitialArray: [],
  busy: false,
  busyLoading: false,
  indicesSelected: []
}

const getters = {
  imageDataArray(state) {
    return state.imageDataArray;
  },
  imageDataUrlsArray(state) {
    return state.imageDataUrlsArray;
  },
  imageFieldOfViewArray(state) {
    return state.imageFieldOfViewArray;
  },
  busy(state) {
    return state.busy;
  },
  busyLoading(state) {
    return state.busyLoading;
  },
  indicesSelected(state) {
    return state.indicesSelected;
  },
  imageFieldOfView(state) {
    return index => state.imageFieldOfViewArray[index];
  },
  imageFieldOfViewInitial(state) {
    return index => state.imageFieldOfViewInitialArray[index];
  },
  imageDataValid(state) {
    return index => state.imageDataArray[index]
        && state.imageDataArray[index].width > 0
        && state.imageDataArray[index].height > 0;
  },
  imageCount(state) {
    return state.imageDataArray.length;
  }
}

const mutations = {

  imageDataArray(state, imageDataArray) {
    state.imageDataArray = imageDataArray;
  },
  imageData(state, imageData) {
    state.imageDataArray.push(imageData);
  },
  _imageDataUrl(state, imageDataUrl) {
    state.imageDataUrlsArray.push(imageDataUrl);
  },
  imageFieldOfView(state, fieldOfView) {
    state.imageFieldOfViewArray.push(fieldOfView);
  },
  imageFieldOfViewInitial(state, fieldOfView) {
    state.imageFieldOfViewInitialArray.push(fieldOfView);
  },
  imageDataUrlsArray(state, imageDataUrlsArray) {
    state.imageDataUrlsArray = imageDataUrlsArray;
  },
  imageFieldOfViewUpdate(state, { index, fieldOfView }) {
    state.imageFieldOfViewArray.splice(index, 1, fieldOfView);
  },
  imageClicked(state, value) {
    if(value == -1) {
      state.indicesSelected = [];
      return;
    }
    const index = state.indicesSelected.findIndex(selectedIndex => selectedIndex == value);
    if(index == -1) {
      state.indicesSelected.push(value);
    }
    else {
      state.indicesSelected.splice(index, 1);
    }
  },
  busy(state, value) {
    state.busy = value;
  },
  busyLoading(state, value) {
    state.busyLoading = value;
  },
  swap(state, { indexFrom, indexTo} ) {
    const tempImageData = state.imageDataArray[indexTo];
    const tempImageDataUrl = state.imageDataUrlsArray[indexTo];
    const tempFieldOfView = state.imageFieldOfViewArray[indexTo];
    const tempFieldOfViewInitial = state.imageFieldOfViewInitialArray[indexTo];

    state.imageDataArray.splice(indexTo, 1, state.imageDataArray[indexFrom]);
    state.imageDataUrlsArray.splice(indexTo, 1, state.imageDataUrlsArray[indexFrom]);
    state.imageFieldOfViewArray.splice(indexTo, 1, state.imageFieldOfViewArray[indexFrom]);
    state.imageFieldOfViewInitialArray.splice(indexTo, 1, state.imageFieldOfViewInitialArray[indexFrom]);

    state.imageDataArray.splice(indexFrom, 1, tempImageData);
    state.imageDataUrlsArray.splice(indexFrom, 1, tempImageDataUrl);
    state.imageFieldOfViewArray.splice(indexFrom, 1, tempFieldOfView);
    state.imageFieldOfViewInitialArray.splice(indexFrom, 1, tempFieldOfViewInitial);
  },
  removeSelected(state) {

    state.imageDataArray = state.imageDataArray.filter((_, index) => !state.indicesSelected.some(val => val == index));
    state.imageDataUrlsArray = state.imageDataUrlsArray.filter((_, index) => !state.indicesSelected.some(val => val == index));
    state.imageFieldOfViewArray = state.imageFieldOfViewArray.filter((_, index) => !state.indicesSelected.some(val => val == index));
    state.imageFieldOfViewInitialArray = state.imageFieldOfViewInitialArray.filter((_, index) => !state.indicesSelected.some(val => val == index));

    state.indicesSelected = [];
  },
  removeAll(state) {
    state.imageDataArray = [];
    state.imageDataUrlsArray = [];
    state.imageFieldOfViewArray = [];
    state.imageFieldOfViewInitialArray = [];

    state.indicesSelected = [];
  }
}

const actions = {

  async imageData({ commit, dispatch, rootGetters }, imageData) {
    if(imageData && rootGetters['worker/ready']) {
      const maxPixelsN = rootGetters['settings/param'](paramTypes.imageCapInput.id);
      if(imageData.width * imageData.height > maxPixelsN) {
        const scaleF = Math.sqrt(maxPixelsN / (imageData.width * imageData.height));
        const height = Math.floor(imageData.height * scaleF);
        const width = Math.floor(imageData.width * scaleF);
        if(rootGetters['worker/ready'])
          await dispatch('worker/setMultiInputImageResized', {
            imageDataSrc: imageData,
            width,
            height }, {
              root: true
              });
        return;
      }
    }

    commit('imageData', imageData);
    dispatch('_imageDataUrl', imageData);

    if(rootGetters['worker/ready']) {
      dispatch('worker/multiStitchResetWorkerData', null, { root: true });
      commit('worker/results/imageData', { name: multiStitchName, imageData: null}, { root: true });
    }
  },

  _imageDataUrl({ commit }, imageData) {
    if(!imageData) commit('_imageDataUrl', null);
    else commit('_imageDataUrl', ImageDataConversion.imageSrcFromImageData(imageData));
  },
  async imageFile(context, file) {

    const wasBusy = context.getters['busy'];

    return new Promise((resolve) => {

      const img = new Image(); 
      img.onload = async () => {
        try {
          const imageData = ImageDataConversion.imageDataFromImageSrc(img);
          await context.dispatch('imageData', imageData);
          
          const fieldOfView = await ExifHelper.getFieldOfViewAsync(img);
          if(fieldOfView) {
            context.commit('imageFieldOfView', fieldOfView);
            context.commit('imageFieldOfViewInitial', fieldOfView);
          }
          else {
            context.commit('imageFieldOfView', 45);
            context.commit('imageFieldOfViewInitial', 45);
          }

          img.onload = null;
        }
        finally {
          if(!wasBusy) context.commit('busy', false);
          resolve();
        }
      }
      if(file) {
        if(!wasBusy) context.commit('busy', true);
        img.src = URL.createObjectURL(file);
      }
    });
  },
  async imageFiles(context, files) {
    try {
      context.commit('busy', true);

      const localFiles = []
      for(const file of files) {
        localFiles.push(file);
      }
      for(const file of localFiles) {
        await context.dispatch('imageFile', file);
      }
    }
    finally {
      context.commit('busy', false);
    }
  },
  removeAllOrSelected(context) {
    if(context.getters['indicesSelected'].length == 0) {
      context.commit('removeAll');
    }
    else {
      context.commit('removeSelected');
    }
  }

}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions
};