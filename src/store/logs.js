let errorTimeoutId = null;

const state = {
  maxMessagesCount: 50,
  messages: [],
  totalCount: 0,
  infoMessafe: '',

  maxErrorMessagesCount: 5,
  errorMessages: [],
  currentErrorMessage: ''
}

const getters = {
  messages(state) {
    return state.messages;
  },
  totalCount(state) {
    return state.totalCount;
  },
  errorMessages(state) {
    return state.errorMessages;
  },
  currentErrorMessage(state) {
    return state.currentErrorMessage;
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
  },
  addErrorMessage(state, message) {
    if(state.maxErrorMessagesCount == state.errorMessages.length) {
      state.errorMessages.shift();
    }
    state.errorMessages.push(new Date().toLocaleTimeString() + ': ' + message);

    if(errorTimeoutId) {
      clearTimeout(errorTimeoutId);
      errorTimeoutId = null;
    }

    state.currentErrorMessage = message;
    errorTimeoutId = setTimeout(() => {
      state.currentErrorMessage = '';
    }, 3000);
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