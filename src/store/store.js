import Vue from 'vue'
import Vuex from 'vuex'

import input from '@/store/input';
import multiInput from '@/store/multiInput';
import worker from '@/store/worker';
import settings from '@/store/settings';
import logs from '@/store/logs';

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
    input,
    multiInput,
    logs
  }
})
