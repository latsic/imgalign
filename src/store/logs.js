
const state = {
  maxMessagesCount: 20,
  messages: [],
  totalCount: 0,
  infoMessafe: ''
}

const getters = {
  messages(state) {
    return state.messages;
  },
  totalCount(state) {
    return state.totalCount;
  }
}

const mutations = {
  addMessage(state, message) {
    if(state.maxMessagesCount == state.messages.length) {
      state.messages.shift();
    }
    state.messages.push(new Date().toLocaleTimeString() + ': ' + message);
    state.totalCount++;
  },
  setInfoMessage(state, message) {
    state.infoMessafe = message;
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