import Vue from 'vue'
import Vuex from 'vuex'

import input from '@/store/input';
import worker from '@/store/worker';
import settings from '@/store/settings';

Vue.use(Vuex)

export default new Vuex.Store({
  //strict: true,
  state: {
  },
  getters: {
  },
  mutations: {
  },
  actions: {
  },
  modules: {
    worker,
    settings,
    input
  }
})
