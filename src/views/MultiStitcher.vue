<template>
  <app-view-layout
    :title="'Multistitcher'"
  >
    <template
      slot="rightElem"
    >
      <div
        :style="{'padding-right': '0.2rem'}"
      >
        <span
          :style="{'white-space': 'nowrap'}"
        >
          detector: <strong>{{ detTypeName }} </strong>
        </span>
      </div>
      
    </template>
    <template
      slot="leftElem"
    >
      <div
        :style="{
          'padding-right': '0.2rem',
          'padding-left': '1rem',
          color: $vuetify.theme.primary,
          cursor: 'pointer'
        }"
        @click="switchSettingsDisplay"
      >
        <span
          :style="{'white-space': 'nowrap'}"
        >
          <strong v-if="$store.getters['settings/multiStitcherSettingsOn']">▲ Settings</strong>
          <strong v-else>▼ Settings</strong>
        </span>
      </div>
      
    </template>

    <div>
    
      <app-action-bar
        :compute="multiStitch"
        :kill="() => $store.dispatch('worker/reload')"
        :compute-success="$store.getters['worker/results/success'](multiStitchName)"
        :worker-ready="$store.getters['worker/ready']"
        :worker-busy-compute="$store.getters['worker/busyCompute']"
        :worker-busy-image="$store.getters['worker/busyImage']"
        :worker-action-info="$store.getters['worker/currentActionInfo']"
        :worker-action-info-error="$store.getters['logs/currentErrorMessage']"
        :input-busy-image="$store.getters['multiInput/busy'] || $store.getters['multiInput/busyLoading']"
        :fail-text="errorText"
        :action-text="'Stitch!'"
        :result-valid="$store.getters['worker/results/imageDataValid'](multiStitchName)"
        :multiple="true"
        :delete-disabled="$store.getters['multiInput/imageCount'] == 0"
        :enable-save-image="true"
        @delete-result="deleteResult"
        @delete-click="deleteAllOrSelectedInputImages"
        @save-result="saveImage"
        @multi-input-files-changed="multiInputFilesChanged"
      />
    
      <div
        v-if="$store.getters['settings/multiStitcherSettingsOn']">

        <v-tabs
        
          :value="$store.getters['settings/activeTabIndexMultiStitcher'] || 0"
          height="30px"
          color="#eeeeee"
          @change="index => $store.dispatch('settings/activeTabIndexMultiStitcher', index)"
        >
          <v-tab
            v-for="(groupKey, index) of groupKeysMultiStitcherSettings"
            :key="index"
            ripple
          >
            {{ paramGroupName(groupKey) }}
          </v-tab>
          <v-tab-item
            v-for="(groupKey, index) of groupKeysMultiStitcherSettings"
            :key=index
            lazy
          >
            <app-multi-stitcher-camera-settings
              v-if="groupKey == 'multiStitcherCamera'"
              :params="params(groupKey)"
              :showFieldOfView="!camEstimationOn"
              :enableFieldOfView="selectedIndices.length > 0"
              :fieldOfView="fieldOfView"
              :fieldOfViewDefaultValue="fieldOfViewDefaultValue"
              @change="paramChanged"
              @fieldOfViewChange="setFieldOfView"
            />

            <app-multi-stitcher-seams-settings
              v-if="groupKey == 'multiStitcherSeams'"
              :params="params(groupKey)"
              @change="paramChanged"
            />

            <app-multi-stitcher-image-settings
              v-if="groupKey == 'multiStitcherImage'"
              :params="params(groupKey)"
              @change="paramChanged"
            />

            <app-multi-stitcher-confidence-settings
              v-if="groupKey == 'multiStitcherConfidence'"
              :params="params(groupKey)"
              @change="paramChanged"
            />

            <app-multi-stitcher-memory-settings
              v-if="groupKey == 'multiStitcherMemory'"
              :params="params(groupKey)"
              @change="paramChanged"
            />

            <app-multi-stitcher-stitch-settings
              v-if="groupKey == 'multiStitcherStitchOrder'"
              :params="params(groupKey)"
              @change="paramChanged"
            />
            
          </v-tab-item>
        </v-tabs>

        <app-view-spacer
          mtop="0px"
        />
      </div>

      <v-layout
        v-if="resultValid"
        justify-center
      >
        <app-image-result
          :image-url="$store.getters['worker/results/imageDataUrl'](multiStitchName)"
          :image-name="multiStitchName"
          :canDrag="false"
        />
      </v-layout>

      <app-multi-input-images
        :imageUrlArray="$store.getters['multiInput/imageDataUrlsArray']"
        :fieldOfViewArray="$store.getters['multiInput/imageFieldOfViewArray']"
        :indicesSelected="$store.getters['multiInput/indicesSelected']"
        @imageClicked="index => $store.commit('multiInput/imageClicked', index)"
        @swap="swapImagesInputImages"
      />

    </div>

  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';
import ViewSpacer from '@/components/layout/ViewSpacer';
import ActionBar from '@/components/common/ActionBar';
import MultiInputImages from '@/components/common/MultiInputImages';
import ImageResult from '@/components/common/ImageResult';
import MultiStitcherCameraSettings from '@/components/settings/MultiStitcherCameraSettings';
import MultiStitcherImageSettings from '@/components/settings/MultiStitcherImageSettings';
import MultiStitcherStitchSettings from '@/components/settings/MultiStitcherStitchSettings';
import MultiStitcherSeamsSettings from '@/components/settings/MultiStitcherSeamsSettings';
import MultiStitcherMemorySettings from '@/components/settings/MultiStitcherMemorySettings';
import MultiStitcherConfidenceSettings from '@/components/settings/MultiStitcherConfidenceSettings';
import { multiStitchName } from '@/models/constants/images';
import { paramTypes, paramGroups, ParamUtils } from '@/models/constants/params';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppActionBar': ActionBar,
    'AppImageResult': ImageResult,
    'AppMultiInputImages': MultiInputImages,
    'AppMultiStitcherImageSettings': MultiStitcherImageSettings,
    'AppMultiStitcherCameraSettings': MultiStitcherCameraSettings,
    'AppMultiStitcherStitchSettings': MultiStitcherStitchSettings,
    'AppMultiStitcherSeamsSettings': MultiStitcherSeamsSettings,
    'AppMultiStitcherMemorySettings': MultiStitcherMemorySettings,
    'AppMultiStitcherConfidenceSettings': MultiStitcherConfidenceSettings
  },
  created() {
    this.$store.dispatch('multiInput/init');
    this.$store.dispatch('worker/load');
  },
  computed: {
    multiStitchName() {
      return multiStitchName;
    },
    selectedIndices() {
      return this.$store.getters['multiInput/indicesSelected'];
    },
    fieldOfView() {
      return this.selectedIndices.length > 0
        ?  this.$store.getters['multiInput/imageFieldOfView'](this.selectedIndices[0]) + ''
        : '';
    },
    fieldOfViewDefaultValue() {
      return this.selectedIndices.length > 0
        ? this.$store.getters['multiInput/imageFieldOfViewInitial'](this.selectedIndices[0]) + ''
        : '';
    },
    resultValid() {
      return this.$store.getters['worker/results/imageDataValid'](multiStitchName);
    },
    errorText() {
      const e = this.$store.getters['worker/error'];
      if(e && e.message) return e.message;
      return 'No match found!';
    },
    detTypeName() {
      return ParamUtils.getParamName(this.$store.getters['settings/param'](paramTypes.detType.id));
    },
    camEstimationOn() {
      return this.$store.getters['settings/param'](paramTypes.multiStitch_camEstimate.id);
    }
  },
  data() {
    return {
      settingsOn: false,
      groupKeysMultiStitcherSettings: [
        'multiStitcherCamera',
        'multiStitcherSeams',
        'multiStitcherImage',
        'multiStitcherConfidence',
        'multiStitcherMemory',
        'multiStitcherStitchOrder'
      ]
    }
  },
  methods: {
    async multiStitch() {

      await this.$store.dispatch('multiInput/reloadFilesFromDiscIf');

      await this.$store.dispatch(
        'worker/computeMultiStitchImageInSteps', {
          images: this.$store.getters['multiInput/imageDataArray'],
          fieldsOfView: this.$store.getters['multiInput/imageFieldOfViewArray'],
          settings: this.$store.getters['settings/settings']
        }
      );      
    },
    deleteWorkerData() {
      //this.$store.commit('worker/results/imageData', { name: multiStitchName, imageData: null });
      this.$store.dispatch('worker/multiStitchResetWorkerData');
    },
    deleteResult() {
      this.$store.commit('worker/results/imageData', { name: multiStitchName, imageData: null });
      //this.$store.dispatch('worker/multiStitchResetWorkerData');
    },
    deleteAllOrSelectedInputImages() {
      this.$store.dispatch('multiInput/removeAllOrSelected');
      this.deleteWorkerData();
    },
    swapImagesInputImages({ indexFrom, indexTo }) {
      this.$store.commit('multiInput/swap', { indexFrom, indexTo });
      this.deleteWorkerData();
    },
    saveImage() {
      this.$store.dispatch('worker/saveResultImage', { name:  multiStitchName, imageFileName: "MultiStitcherImage.png" });
    },
    async multiInputFilesChanged(files) {
      this.deleteWorkerData();
      await this.$store.dispatch('multiInput/imageFiles', files);
    },
    params(groupKey) {
      return this.$store.getters['settings/settings'].paramsByGroupKey(groupKey);
    },
    paramGroupName(key) {
      return paramGroups[key].name;
    },
    switchSettingsDisplay() {
      if(this.$store.getters['settings/multiStitcherSettingsOn']) {
        this.$store.dispatch('settings/multiStitcherSettingsOn', false);
      }
      else {
        this.$store.dispatch('settings/multiStitcherSettingsOn', true);
      }
    },
    async paramChanged(obj) {
      await this.$store.dispatch('settings/param', obj);
    },
    setFieldOfView(value) {

      this.selectedIndices.forEach(index => {

        if(!value) {
          this.$store.commit(
            'multiInput/imageFieldOfViewUpdate', {
              index,
              fieldOfView: this.$store.getters['multiInput/imageFieldOfViewInitial'](index)
          });
        }
        else {
          this.$store.commit(
            'multiInput/imageFieldOfViewUpdate', {
              index,
              fieldOfView: +value
          });
        }
      });
    }
  }
}
</script>

<style>

</style>