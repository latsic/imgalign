<template>
  <app-view-layout
    :title="'Stitcher'"
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
          <strong v-if="$store.getters['settings/stitcherSettingsOn']">▲ Settings</strong>
          <strong v-else>▼ Settings</strong>
        </span>
      </div>
      
    </template>

    <div>
    
      <app-action-bar
        :compute="stitch"
        :compute-success="$store.getters['worker/results/success'](stitchName)"
        :worker-ready="$store.getters['worker/ready']"
        :worker-busy-compute="$store.getters['worker/busyCompute']"
        :worker-busy-image="$store.getters['worker/busyImage']"
        :worker-action-info="$store.getters['worker/currentActionInfo']"
        :worker-action-info-error="$store.getters['logs/currentErrorMessage']"
        :input-busy-image="$store.getters['input/busyFixedImage'] || $store.getters['input/busyMovingImage']"
        :fail-text="errorText"
        :action-text="'Stitch!'"
        :result-valid="$store.getters['worker/results/imageDataValid'](stitchName)"
        :inputButtonsTitle="'First image / second image'"
        @delete-result="deleteResult"
        @load-default-images="loadDefaultImages"
      />
      
      <div
        v-if="$store.getters['settings/stitcherSettingsOn']">
      
        <v-tabs
        
          :value="$store.getters['settings/activeTabIndexStitcher'] || 0"
          height="30px"
          show-arrows
          color="#eeeeee"
          @change="index => $store.dispatch('settings/activeTabIndexStitcher', index)"
        >
          <v-tab
            v-for="(groupKey, index) of groupKeysStitcherSettings"
            :key="index"
            ripple
          >
            {{ paramGroupName(groupKey) }}
          </v-tab>
          <v-tab-item
            v-for="(groupKey, index) of groupKeysStitcherSettings"
            :key=index
            lazy
          >
            <app-stitcher-common-settings
              v-if="groupKey == 'stitcherCommon'"
              :params="params(groupKey)"
              @change="paramChanged"
            />
            <app-stitcher-orientation-settings
              v-else-if="groupKey == 'stitcherOrientation'"
              :params="params(groupKey)"
              :fieldOfViewInitial1="fieldOfViewInitialValue1"
              :fieldOfViewInitial2="fieldOfViewInitialValue2"
              :getParamById="(id) => $store.getters['settings/settings'].getParamById(id)"
              @change="paramChanged"
            />
            <app-stitcher-projection-settings
              v-else-if="groupKey == 'stitcherProjection'"
              :params="params(groupKey)"
              :settings="$store.getters['settings/setttings']"
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
          :image-url="$store.getters['worker/results/imageDataUrl'](stitchName)"
          :image-name="stitchName"
          :projected="$store.getters['worker/results/stitcherProjected']"
          :field-of-view="$store.getters['worker/results/stitcherFieldOfView']"
        />
      </v-layout>

      <app-input-images
        :polygons="$store.getters['input/polygons']"
        @addPolygonPoint="addPolygonPt"
      />

    </div>

  </app-view-layout>
</template>

<script>


import ViewLayout from '@/components/layout/ViewLayout';
import ViewSpacer from '@/components/layout/ViewSpacer';
import ActionBar from '@/components/common/ActionBar';
import InputImages from '@/components/common/InputImages';
import ImageResult from '@/components/common/ImageResult';
import StitcherCommonSettings from '@/components/settings/StitcherCommonSettings';
import StitcherOrientationSettings from '@/components/settings/StitcherOrientationSettings';
import StitcherProjectionSettings from '@/components/settings/StitcherProjectionSettings';
import { fixedImageName, movingImageName, stitchName } from '@/models/constants/images';
import { paramTypes, paramGroups, ParamUtils } from '@/models/constants/params';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppActionBar': ActionBar,
    'AppInputImages': InputImages,
    'AppImageResult': ImageResult,
    'AppStitcherCommonSettings': StitcherCommonSettings,
    'AppStitcherOrientationSettings': StitcherOrientationSettings,
    'AppStitcherProjectionSettings': StitcherProjectionSettings
  },
  created() {
    this.$store.dispatch('input/init');
    this.$store.dispatch('worker/load');
  },
  computed: {
   
    stitchName() {
      return stitchName;
    },
    resultValid() {
      return this.$store.getters['worker/results/imageDataValid'](stitchName);
    },
    errorText() {
      const e = this.$store.getters['worker/error'];
      if(e && e.message) return e.message;
      return 'No match found!';
    },
    detTypeName() {
      return ParamUtils.getParamName(this.$store.getters['settings/param'](paramTypes.detType.id));
    },
    pathPtsFixedImage() {
      return this.$store.getters['input/pathPtsFixedImage'];
    },
    pathPtsMovingImage() {
      return this.$store.getters['input/pathPtsMovingImage'];
    },
    fieldOfViewInitialValue1() {
      return this.$store.getters['input/imageFieldOfView'](fixedImageName);
    },
    fieldOfViewInitialValue2() {
      return this.$store.getters['input/imageFieldOfView'](movingImageName);
    }
  },
  data() {
    return {
      settingsOn: false,
      groupKeysStitcherSettings: ['stitcherCommon', 'stitcherOrientation', 'stitcherProjection'],
    }
  },
  methods: {
    async stitch() {

      await this.$store.dispatch('worker/computeStitchedImage', {
        fixedImage: this.$store.getters['input/imageData'](fixedImageName),
        movingImage: this.$store.getters['input/imageData'](movingImageName),
        fixedImagePolygonPts: this.$store.getters['input/polygonClosedPts'](fixedImageName),
        movingImagePolygonPts: this.$store.getters['input/polygonClosedPts'](movingImageName),
        settings: this.$store.getters['settings/settings']
      });

      if(this.$store.getters['worker/results/success'](stitchName)) {
        await this.$store.dispatch('worker/displayStitchedImage');
      }
    },
    deleteResult() {
      this.$store.commit('worker/results/imageData', { name: stitchName, imageData: null });
      this.$store.dispatch('worker/resetWorkerData');
    },
    loadDefaultImages() {
      this.$store.dispatch('input/loadDefaultImages');
      this.$store.dispatch('worker/resetWorkerData');
    },
    addPolygonPt(name, pt) {
      const floodFillTolerance = this.$store.getters['settings/param'](paramTypes.floodFillTolerance.id);
      this.$store.commit('input/addPolygonPt', { name, pt, floodFillTolerance });
    },
    params(groupKey) {
      return this.$store.getters['settings/settings'].paramsByGroupKey(groupKey);
    },
    paramGroupName(key) {
      return paramGroups[key].name;
    },
    async paramChanged(obj) {
      await this.$store.dispatch('settings/param', obj);

      if(!this.$store.getters['worker/results/imageDataValid'](stitchName)) {
        return;
      }
      await this.$store.dispatch('worker/displayStitchedImage');
    },
    switchSettingsDisplay() {
      if(this.$store.getters['settings/stitcherSettingsOn']) {
        this.$store.dispatch('settings/stitcherSettingsOn', false);
      }
      else {
        this.$store.dispatch('settings/stitcherSettingsOn', true);
      }
    }
  }
}
</script>

<style scoped>
  .slider {
    margin-top: 0;
    margin-bottom: 0;
    border: 2px solid;
    border-radius: 0.3rem 0.3rem;
    padding: 0.7rem 0.7rem
  }
</style>