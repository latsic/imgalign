<template>
  <app-view-layout
    :title="'Compare feature detectors'"
  >
    <app-action-bar
      :compute="compare"
      :compute-success="$store.getters['worker/results/success'](compareName)"
      :worker-ready="$store.getters['worker/ready']"
      :worker-busy-compute="$store.getters['worker/busyCompute']"
      :worker-busy-image="$store.getters['worker/busyImage']"
      :worker-action-info="$store.getters['worker/currentActionInfo']"
      :worker-action-info-error="$store.getters['logs/currentErrorMessage']"
      :input-busy-image="$store.getters['input/busyFixedImage'] || $store.getters['input/busyMovingImage']"
      :fail-text="errorText"
      :action-text="'Compare!'"
      :action-button-disabled-condition="detTypeValues.length == 0"
      :result-valid="$store.getters['worker/results/imageDataValid'](compareName)"
      @delete-result="deleteResult"
    />

    <app-compare
      :params="settings.paramsByGroupKey('compare')"
      @change="changed"
    />

    <v-layout
      v-if="resultValid"
      row
      wrap
      align-baseline
      justify-center
    >
      <app-image-result
        :image-url="$store.getters['worker/results/imageDataUrl'](compareName)"
        :image-name="compareName"
      />
    </v-layout>
    <v-layout
      v-if="$store.getters['worker/results/imageDataValid'](compareName)"
      justify-space-around
    >
      <v-flex
        v-for="(name, index) of resultInfos"
        :key="index"
      >
        {{ name }}
      </v-flex>
    </v-layout>

    <app-view-spacer
      v-if="true"
    />

    <app-input-images
      :polygons="$store.getters['input/polygons']"
      @addPolygonPoint="(name, pt) => $store.commit('input/addPolygonPt', { name, pt })"
    />

  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';
import ViewSpacer from '@/components/layout/ViewSpacer';
import ActionBar from '@/components/common/ActionBar';
import Compare from '@/components/compare/Compare';
import InputImages from '@/components/common/InputImages';
import ImageResult from '@/components/common/ImageResult';
import { fixedImageName, movingImageName, compareName } from '@/models/constants/images';
import { paramTypes } from '@/models/constants/params';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppCompare': Compare,
    'AppActionBar': ActionBar,
    'AppInputImages': InputImages,
    'AppImageResult': ImageResult
  },
  created() {
    this.$store.dispatch('input/init');
    this.$store.dispatch('worker/load');
  },
  computed: {
    settings() {
      return this.$store.getters['settings/settings'];
    },
    resultInfos() {

      const resultSettings = this.$store.getters['worker/results/settings'](compareName);

      let names = resultSettings.getParamValueNamesById(paramTypes.compareDetTypes.id)

      let i = 0;
      for(let time of this.$store.getters['worker/results/compareTimeUsedPerDet']) {
        names[i++] +=  ` / ${time} ms`;
      }
      i = 0;
      for(let keyPointsCount of this.$store.getters['worker/results/compareKeyPointsCountPerDet']) {
        names[i++] += ` / ${keyPointsCount} pts`
      }

      return names;
    },
    detTypeValues() {
      return this.settings.getParamValuesById(paramTypes.compareDetTypes.id);
    },
    imageType() {
      return this.settings.getParamValueById(paramTypes.compareImageType.id);
    },
    compareName() {
      return compareName;
    },
    resultValid() {
      return this.$store.getters['worker/results/imageDataValid'](compareName);
    },
    errorText() {
      const e = this.$store.getters['worker/error'];
      if(e && e.message) return e.message;
      return 'Unknown error';
    },
  },
  methods: {
    async compare() {
      
      // eslint-disable-next-line no-console
      console.assert(
           this.$store.getters['input/imageDataValid'](fixedImageName)
        && this.$store.getters['input/imageDataValid'](movingImageName),
        "Invalid InputData in Matcher.match");

      await this.$store.dispatch(
        'worker/computeCompareImage', {
          fixedImage: this.$store.getters['input/imageData'](fixedImageName),
          movingImage: this.$store.getters['input/imageData'](movingImageName),
          fixedImagePolygonPts: this.$store.getters['input/polygonClosedPts'](fixedImageName),
          movingImagePolygonPts: this.$store.getters['input/polygonClosedPts'](movingImageName),
          settings: this.settings
        }
      );
    },
    deleteResult() {
      this.$store.commit('worker/results/imageData', { name: compareName, imageData: null });
      this.$store.dispatch('worker/resetWorkerData');
    },
    changed(obj) {
      this.$store.dispatch('settings/param', obj);
    }
  }
}
</script>

<style scoped>

</style>
