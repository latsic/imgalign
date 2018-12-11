
import { LocalStorageUtils } from '@/utilities/LocalStorageUtils';
import { Settings } from '@/models/Settings';

const state = {
  settings: new Settings(),
  defaultSettings: null,
  activeTabIndex: null
}

const getters = {
  param(state) {
    return id => state.settings.getParamValueById(id);
  },
  settings(state) {
    return state.settings;
  },
  equalsDefault(state) {
    return state.settings.equals(state.defaultSettings);
  },
  idValueArrExcludingDefaults(state) {
    return state.settings.getIdValueArrExcludingDefaults();
  },
  activeTabIndex(state) {
    return state.activeTabIndex;
  },
  getCurrentDetTypeGroupKey(state) {
    return state.settings.getCurrentDetTypeGroupKey();
  }
}

const mutations = {
  param(state, { id, value }) {
    state.settings.setParamValueById(id, value);
  },
  params(state, idValueArr) {
    state.settings.setByIdValueArr(idValueArr);
  },
  default(state) {
    state.settings = new Settings();
  },
  defaultSettings(state, defaultSettings) {
    state.defaultSettings = Object.freeze(defaultSettings);
  },
  activeTabIndex(state, value) {
    state.activeTabIndex = value;
  }
}

const actions = {
  init(context) {
    context.commit('defaultSettings', context.getters['settings'].getIdValueArr());
    context.commit('params', LocalStorageUtils.getIdValueArr(context.getters['settings'].getIdArr()));
  },
  param({ commit, getters }, { id, value }) {

    const settings = getters['settings'];

    commit('param', { id, value });

    LocalStorageUtils.saveIdValue(id, value, settings.getParamDefaultValueById(id));

    const relatingIdValue = settings.getRelatingIdValue(id, value);
    if(relatingIdValue) {
      commit('param', { id: relatingIdValue.id, value: relatingIdValue.value });
      
      LocalStorageUtils.saveIdValue(
        relatingIdValue.id,
        relatingIdValue.value,
        settings.getParamDefaultValueById(relatingIdValue.id));
    }
  },
  default({ commit, getters }) {
    commit('default');
    const arr = getters['settings'].getIdValueDefaultArr();
    LocalStorageUtils.saveIdValueArr(arr);
  },
  activeTabIndex({ commit }, value) {
    commit('activeTabIndex', value);
  }
}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions
};
