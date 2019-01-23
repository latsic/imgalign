<template>
  <div>
    <div>

      <v-layout
        text-xs-left
        caption
        justify-space-between
        pr-2
        align-center
      >
        <span v-if="!multiple">
          Template image / image to align
        </span>
        <span v-else>
          Choose input
        </span>
        
        <span
          v-if="!workerReady"
          :style="{'color': $vuetify.theme.primary}"
        >
          Loading OpenCV {{ timeStr(currentLoadingTime) }}
        </span>
        <span
          v-else-if="workerBusyCompute || workerBusyImage && !showStatusInfo && currentComputeTime && workerActionInfoMessage"
          :style="{'color': !workerActionInfoError ? $vuetify.theme.primary : $vuetify.theme.error}"
        >
          {{ timeStr(currentComputeTime) + ', ' + workerActionInfoMessage }}
        </span>
        <span
          v-else-if="(inputBusyImage || resultValid) && !showStatusInfo && transitionDone && workerActionInfoMessage"
          :style="{'color': !workerActionInfoError ? $vuetify.theme.primary : $vuetify.theme.error}"
          >
          {{ workerActionInfoMessage }} ...
        </span>
        <span
          v-else-if="resultValid && !showStatusInfo && transitionDone"
          :style="{
            'color': 'red',
            cursor: 'pointer',
            'padding-right': '0.2rem'
          }"
          @click="$emit('delete-result')"
        >
          <strong>▶ Clear image</strong>
        </span>

        <transition
          name="status-info"
          @after-enter="afterEnter"
          @after-leave="afterLeave"
        >
          <span
            v-if="showStatusInfo && workerActionInfoError && computeSuccess === 1"
            :style="{'color': $vuetify.theme.error}"
          >
            <strong>{{ workerActionInfoError }}, time {{ timeStr(computeTimeTotal) }}</strong>
          </span>
          <span
            v-else-if="showStatusInfo && computeSuccess === 1"
            :style="{'color': $vuetify.theme.success}"
          >
            <strong>Success, time {{ timeStr(computeTimeTotal) }}</strong>
          </span>
          
          <span
            v-else-if="showStatusInfo && computeSuccess === 2"
            :style="{'color': $vuetify.theme.error}"
          >
            <strong>Low confidence result, time {{ timeStr(computeTimeTotal) }}</strong>
          </span>
          <span
            v-else-if="showStatusInfo && computeSuccess === 0"
            :style="{'color': $vuetify.theme.error}"
          >
            <strong>{{ failText }}</strong>
          </span>
          <span
            v-else-if="showStatusInfo && workerErrorMessage"
            :style="{'color': $vuetify.theme.error}"
          >
            <strong>{{ workerErrorMessage }}</strong>
          </span>
        </transition>
          
      </v-layout>



      <v-layout
        row
        wrap
        align-center
      >
        <v-flex
          d-flex
        >
          <v-layout v-if="!multiple">
            <app-upload-button
              v-for="(imageName, index) of $store.getters['input/allImageNames']"
              :key="imageName"
              :unique-id="true"
              :style="{'padding': '0 0.2rem 0 0'}"
              :title="`${index + 1} ...`"
              :disabled="workerBusy || inputBusyImage"
              :accept="'image/*'"
              :multiple="multiple"
              small
              :file-changed-callback="file => onFileChanged(imageName, file)"
              @file-selection-started="$store.commit('input/busy', { name: imageName, value: true })"
              @file-selection-ended="$store.commit('input/busy', { name: imageName, value: false })"
            />
          </v-layout>
          <v-layout v-else>
            <app-upload-button
              :key="'multiStitch'"
              :unique-id="true"
              :style="{'padding': '0 0.2rem 0 0'}"
              :title="`Images ...`"
              :disabled="workerBusy || inputBusyImage"
              :accept="'image/*'"
              :multiple="multiple"
              small
              :file-changed-callback="files => onMultiFileChanged(files)"
              @file-selection-started="$store.commit('multiInput/busyLoading', true)"
              @file-selection-ended="$store.commit('multiInput/busyLoading', false)"
            />
            <v-btn
              :style="{
                'padding': '0 0.2rem 0 0',
                margin: '0',
                'text-transform': 'none !important'
              }"
              small
              color="primary"
              :disabled="deleteDisabled"
              @click="$emit('delete-click');"
            >
              Delete
            </v-btn>
          </v-layout>
          
        </v-flex>
        
        <v-flex>
          <v-layout
            justify-end
            align-center
          >
            <v-btn
              :style="{
                'text-transform': 'none !important',
                'position': 'relative'
              }"
              :color="(workerBusyCompute && kill != null) ? 'error' : 'primary'"
              :disabled="computeButtonDisabled"
              small
              @click="(workerBusyCompute && kill != null) ? runKill() : runCompute()"
            >
              {{ (workerBusyCompute && kill != null) ? 'Abort' : actionText }}
            </v-btn>
          </v-layout>
          
        </v-flex>
      </v-layout>
      <app-view-spacer
        mtop="0.5rem"
      />
    </div>
  </div>  
</template>

<script>

import UploadButton from '@/components/gui/UploadButton';
import ViewSpacer from '@/components/layout/ViewSpacer';

export default {
  components: {
    'AppUploadButton': UploadButton,
    'AppViewSpacer': ViewSpacer,
  },
  props: {
    compute: {
      type: Function,
      required: true
    },
    kill: {
      type: Function,
      default: null,
    },
    computeSuccess: {
      validator(value) {
        return [0, 1, 2, null].indexOf(value) !== -1
      },
      required: true
    },
    workerReady: {
      type: Boolean,
      required: true
    },
    workerBusyCompute: {
      type: Boolean,
      required: true
    },
    workerBusyImage: {
      type: Boolean,
      required: true
    },
    workerActionInfo: {
      type: String,
      default: ''
    },
    workerActionInfoError: {
      type: String,
      required: true
    },
    workerlastCompletedActionTime: {
      type: Number,
      default: 0 
    },
    inputBusyImage: {
      type: Boolean,
      required: true
    },
    failText: {
      type: String,
      default: 'No match found!'
    },
    actionText: {
      type: String,
      default: 'Match!'
    },
    actionButtonDisabledCondition: {
      type: Boolean,
      default: false
    },
    resultValid: {
      type: Boolean,
      default: false
    },
    multiple: {
      type: Boolean,
      default: false
    },
    deleteDisabled: {
      type: Boolean,
      default: false
    }
  },
  created() {
    this.startIntervalCurrentLoadingTime();
  },
  data() {
    return {
      showStatusInfo: false,
      transitionDone: true,
      computeTimeTotal: 0,
      killCalled: false
    }
  },
  computed: {
    workerErrorMessage() {
      const error = this.$store.getters['worker/error'];
      if(!error) return null;
      return error.message || 'Error!'
    },
    workerActionInfoMessage() {

      const msg = this.workerActionInfoError
        ? this.workerActionInfoError
        : this.workerActionInfo;

      if(!msg && this.inputBusyImage) {
        return 'Loading image/s'
      }

      return msg && msg.length > 50
        ? msg.substring(0, 50)
        : msg;
    },
    workerBusy() {
      return this.workerBusyCompute || this.workerBusyImage;
    },
    computeButtonDisabled() {
      return this.actionButtonDisabledCondition
      || this.showStatusInfo
      || !this.workerReady
      || (!this.multiple && !this.$store.getters['input/allImageDataValid'])
      || (this.multiple && this.$store.getters['multiInput/imageCount'] <= 1)
      //|| this.workerBusy
      || (this.workerBusyCompute && this.kill == null)
      || (this.multiple && this.inputBusyImage);
    },
    currentComputeTime() {
      return this.$store.getters['worker/currentComputeTime'];
    },
    currentLoadingTime() {
      return this.$store.getters['worker/currentLoadingTime'];
    }
  },
  methods: {
    onFileChanged(name, file) {
      this.$store.dispatch('input/imageFile', { name, file });
    },
    async onMultiFileChanged(files) {
      this.$store.dispatch('multiInput/imageFiles', files);
    },
    afterEnter() {
      this.showStatusInfo = false;
      this.transitionDone = false;
    },
    afterLeave() {
      this.transitionDone = true;
      this.setCurrentComputeTime(null);
    },
    runKill() {
      this.killCalled = true;
      this.kill();
    },
    async runCompute() {
      this.killCalled = false;
      this.showStatusInfo = false;
      this.startIntervalCurrentComputeTime();
      const startTime = new Date();

      await this.compute();

      this.computeTimeTotal = new Date() - startTime;
      if(!this.killCalled) {
        this.showStatusInfo = true;
        setTimeout(() => {
          // just to be on the save side
          this.showStatusInfo = false;
        }, 10000);
      }    
    },
    startIntervalCurrentComputeTime() {

      const startTime = new Date();
      this.setCurrentComputeTime(0);

      if(!this.showStatusInfo) {
        const intervalId = setInterval(() => {
          this.setCurrentComputeTime(new Date() - startTime);
          if(this.showStatusInfo || this.killCalled) {
            clearInterval(intervalId);
            if(this.killCalled) this.killCalled = false;
          }
        }, 100);
      }
    },
    startIntervalCurrentLoadingTime() {

      const startTime = new Date();
      this.setCurrentLoadingTime(0);

      if(!this.workerReady) {
        const intervalId = setInterval(() => {
          this.setCurrentLoadingTime(new Date() - startTime);
          if(this.workerReady) {
            clearInterval(intervalId);
            this.setCurrentLoadingTime(null);
          }
        }, 150);
      }
    },
    timeStr(timeMs) {
      if(timeMs <= 0) return '';
      if(timeMs < 1000) {
        return timeMs + ' ms';
      }
      if(timeMs < 10000) {
        return (timeMs / 1000.0).toFixed(3) + ' s';
      }
      if(timeMs < 100000) {
        return (timeMs / 1000.0).toFixed(2) + ' s';
      }
      if(timeMs < 600000) {
        return (timeMs / 1000.0).toFixed(1) + ' s';
      }
      return `${(timeMs / 600000).toFixed(0)} m, ${(timeMs % 600000)} s`;
    },
    setCurrentComputeTime(value) {
      this.$store.commit('worker/currentComputeTime', value);
    },
    setCurrentLoadingTime(value) {
      this.$store.commit('worker/currentLoadingTime', value);
    }
  }
}
</script>

<style scoped>
  .status-info-enter {
    opacity: 1;
  }
  .status-info-enter-to {
    opacity: 0.0;
  }
  .status-info-leave{
    opacity: 0;
  }
  .status-info-leave-active {
    transition: opacity 0s;
  }
  .status-info-enter-active {
    transition: opacity 6s ease-in;
  }
</style>
