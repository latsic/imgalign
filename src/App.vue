<template>
  <div id="app">
    <v-app>
      <v-content>
        <v-container
          pl-0
          pr-0
        >
          <v-layout
            align-center
            justify-center
          >
            <v-flex>
              <div id="nav">
                <app-navbar
                  :busyWorker="$store.getters['worker/busy']"
                  :busyInput="$store.getters['input/busyAny'] || $store.getters['multiInput/busy']"
                  :workerReady="$store.getters['worker/ready']"
                />
              </div>
              <router-view />
            </v-flex>
          </v-layout>
        </v-container>
      </v-content>
    </v-app>
  </div>
</template>

<script>

import Navbar from '@/components/navigation/Navbar';
import { Logger } from '@/loggers/Logger';
import { paramTypes } from '@/models/constants/params';


export default {
  components: {
    'appNavbar': Navbar,
  },
  created() {
    this.$store.dispatch('settings/init');
    Logger.init(
      () => this.$store.getters['settings/param'](paramTypes.logInfoEnabled.id),
      () => this.$store.getters['settings/param'](paramTypes.logErrorEnabled.id),
      () => this.$store.getters['settings/param'](paramTypes.logAssertEnabled.id),
      () => this.$store.getters['settings/param'](paramTypes.logExternEnabled.id),
      (msg) => this.$store.commit('logs/addMessage', msg),
      (msg) => this.$store.commit('worker/currentActionInfo', msg));
  }
}
</script>

<style>
  #app {
    font-family: "Avenir", Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #2c3e50;
    
    /* margin-top: 60px; */
  }
  html {
    overflow-y: auto;
  }
  
</style>

