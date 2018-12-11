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
        <span>
          Template image / image to align
        </span>
        
        <span
          v-if="!workerReady"
          :style="{'color': $vuetify.theme.primary}"
        >
          Loading OpenCV {{ currentLoadingTime }} ms
        </span>
        <span
          v-else-if="workerBusy && !showStatusInfo"
          :style="{'color': $vuetify.theme.primary}"
        >
          {{ computeText + ' ' + currentComputeTime }} ms
        </span>
        <span
          v-else-if="resultValid && !showStatusInfo && transitionDone"
          :style="{
            'color': 'red',
            cursor: 'pointer',
            'padding-right': '0.2rem'
          }"
          @click="$emit('deleteResult')"
        >
          <strong>▶ Clear image</strong>
        </span>

        <transition
          name="status-info"
          @after-enter="afterEnter"
          @after-leave="afterLeave"
        >
          <span
            v-if="showStatusInfo && computeSuccess === 1"
            :style="{'color': $vuetify.theme.success}"
          >
            <strong>Success, time {{ computeTime }} ms</strong>
          </span>
          
          <span
            v-else-if="showStatusInfo && computeSuccess === 2"
            :style="{'color': $vuetify.theme.error}"
          >
            <strong>Low confidence result, time {{ computeTime }} ms</strong>
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
          <v-layout>
            <app-upload-button
              v-for="(imageName, index) of $store.getters['input/allImageNames']"
              :key="imageName"
              :unique-id="true"
              :style="{'padding': '0 0.2rem 0 0'}"
              :title="`${index + 1} ...`"
              :disabled="workerBusy"
              :accept="'image/*'"
              small
              :file-changed-callback="file => onFileChanged(imageName, file)"
            />
          </v-layout>
        </v-flex>
        
        <v-flex>
          <v-layout
            justify-end
            align-center
          >
            <v-btn
              :style="{'text-transform': 'none !important', 'position': 'relative'}"
              :disabled="actionButtonDisabledCondition || showStatusInfo || !workerReady || !$store.getters['input/allImageDataValid'] || workerBusy"
              small
              color="primary"
              @click="runCompute"
            >
              {{ actionText }}
              <div
                :style="{position: 'absolute'}"
              >
                <app-spinner
                  v-if="!workerReady || workerBusy"
                  :color="$vuetify.theme.primary"
                  :style="{transform:'translateX(-0%)'}"
                />
              </div>
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
import Spinner from '@/components/gui/Spinner';

export default {
  components: {
    'AppUploadButton': UploadButton,
    'AppViewSpacer': ViewSpacer,
    'AppSpinner': Spinner,
  },
  props: {
    compute: {
      type: Function,
      required: true
    },
    computeSuccess: {
      validator(value) {
        return [0, 1, 2, null].indexOf(value) !== -1
      },
      required: true
    },
    computeTime: {
      validator(value) {
        return value == null || value instanceof Date || typeof value == 'number';
      },
      required: true
    },
    workerReady: {
      type: Boolean,
      required: true
    },
    workerBusy: {
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
    computeText: {
      type: String,
      default: 'Calculating alignment'
    },
    actionButtonDisabledCondition: {
      type: Boolean,
      default: false
    },
    resultValid: {
      type: Boolean,
      default: false
    }
  },
  created() {
    this.startIntervalCurrentLoadingTime();
  },
  data() {
    return {
      currentComputeTime: 0,
      showStatusInfo: false,
      transitionDone: true,
      currentLoadingTime: 0
    }
  },
  computed: {
    workerErrorMessage() {
      const error = this.$store.getters['worker/error'];
      if(!error) return null;
      return error.message || 'Error!'
    }
  },
  methods: {
    onFileChanged(name, file) {
      this.$store.dispatch('input/imageFile', { name, file });
    },
    afterEnter() {
      this.showStatusInfo = false;
      this.transitionDone = false;
    },
    afterLeave() {
      this.transitionDone = true;
    },
    async runCompute() {

      this.showStatusInfo = false;
      this.startIntervalCurrentComputeTime();

      await this.compute();

      this.showStatusInfo = true;
      setTimeout(() => {
        // just to be on the save side
        this.showStatusInfo = false;
      }, 10000);
    },
    startIntervalCurrentComputeTime() {

      const startTime = new Date();
      this.currentComputeTime = 0;

      if(!this.showStatusInfo) {
        const intervalId = setInterval(() => {
          this.currentComputeTime = new Date() - startTime;
          if(this.showStatusInfo) {
            clearInterval(intervalId);
          }
        }, 100);
      }
    },
    startIntervalCurrentLoadingTime() {

      const startTime = new Date();
      this.currentLoadingTime = 0;

      if(!this.workerReady) {
        const intervalId = setInterval(() => {
          this.currentLoadingTime = new Date() - startTime;
          if(this.workerReady) {
            clearInterval(intervalId);
          }
        }, 150);
      }
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
