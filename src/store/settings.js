
import { LocalStorageUtils } from '@/utilities/LocalStorageUtils';
import { Settings } from '@/models/Settings';

const state = {
  settings: new Settings(),
  defaultSettings: null,
  activeTabIndexDetector: null,
  activeTabIndexStitcher: null,
  stitcherSettingsOn: false,
  activeTabIndexMultiStitcher: false,
  multiStitcherSettingsOn: false
}

const getters = {
  param(state) {
    return id => state.settings.getParamValueById(id);
  },
  paramDefault(state) {
    return id => state.settings.getParamDefaultValueById(id);
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
  activeTabIndexDetector(state) {
    return state.activeTabIndexDetector;
  },
  getCurrentDetTypeGroupKey(state) {
    return state.settings.getCurrentDetTypeGroupKey();
  },
  activeTabIndexStitcher(state) {
    return state.activeTabIndexStitcher;
  },
  stitcherSettingsOn(state) {
    return state.stitcherSettingsOn;
  },
  multiStitcherSettingsOn(state) {
    return state.multiStitcherSettingsOn;
  },
  activeTabIndexMultiStitcher(state) {
    return state.activeTabIndexMultiStitcher;
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
  activeTabIndexDetector(state, value) {
    state.activeTabIndexDetector = value;
  },
  activeTabIndexStitcher(state, value) {
    state.activeTabIndexStitcher = value;
  },
  stitcherSettingsOn(state, value) {
    state.stitcherSettingsOn = value;
  },
  multiStitcherSettingsOn(state, value) {
    state.multiStitcherSettingsOn = value;
  },
  activeTabIndexMultiStitcher(state, value) {
    state.activeTabIndexMultiStitcher = value;
  }
}

const actions = {
  init(context) {

    const settings = context.getters['settings'];
    context.commit('defaultSettings', settings.getIdValueArr());

    let idValueArr = LocalStorageUtils.getIdValueArr(settings.getIdArr());
    idValueArr = settings.removeInvalidParams(idValueArr);
    context.commit('params', idValueArr);
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
  activeTabIndexDetector({ commit }, value) {
    commit('activeTabIndexDetector', value);
  },
  activeTabIndexStitcher({ commit }, value) {
    commit('activeTabIndexStitcher', value);
  },
  stitcherSettingsOn({ commit }, value) {
    commit('stitcherSettingsOn', value);
  },
  multiStitcherSettingsOn({ commit }, value) {
    commit('multiStitcherSettingsOn', value);
  },
  activeTabIndexMultiStitcher({ commit }, value) {
    commit('activeTabIndexMultiStitcher', value);
  }
}

export default {
  namespaced: true,
  state,
  getters,
  mutations,
  actions
};
