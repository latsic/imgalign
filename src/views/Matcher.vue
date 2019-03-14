<template>
  <app-view-layout
    :title="'Matcher'"
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

    <div>
    
      <app-action-bar
        :compute="match"
        :compute-success="$store.getters['worker/results/success'](matchName)"
        :worker-ready="$store.getters['worker/ready']"
        :worker-busy-compute="$store.getters['worker/busyCompute']"
        :worker-busy-image="$store.getters['worker/busyImage']"
        :worker-action-info="$store.getters['worker/currentActionInfo']"
        :worker-action-info-error="$store.getters['logs/currentErrorMessage']"
        :input-busy-image="$store.getters['input/busyFixedImage'] || $store.getters['input/busyMovingImage']"
        :fail-text="errorText"
        :action-text="'Match!'"
        :result-valid="$store.getters['worker/results/imageDataValid'](matchName)"
        :inputButtonsTitle="'Template image / align image'"
        @delete-result="deleteResult"
        @load-default-images="loadDefaultImages"
      />

      <v-layout
        v-if="resultValid"
        row
        wrap
        align-center
      >
        <v-flex
          xs6
          pr-3
          pb-2
        >
          <v-select
            placeholder="Display Type"
            :items="outputImageTypes"
            :value="$store.getters['worker/results/matcherImageType']"
            :disabled="$store.getters['worker/busy']"
            single-line
            hide-details
            outline
            dense
            @change="imageSelectionChanged"
          />
        </v-flex>
        <v-flex
          xs6
          pl-3
          pb-2
        >
          <v-slider
            v-if="blendSliderVisible"
            always-dirty
            class="slider"
            label="Blend"
            inverse-label
            hide-details
            max="1"
            min="0"
            step="0.01"
            :style="{'border-color': $vuetify.theme.accent}"
            :disabled="$store.getters['worker/busy']"
            :value="$store.getters['worker/results/matcherImageBlend']"
            @change="blendValueChanged"
          />
        </v-flex>
        <v-flex>
          <app-image-result
            :image-url="$store.getters['worker/results/imageDataUrl'](matchName)"
            :image-name="matchName"
          />
        </v-flex>
      </v-layout>

      <app-view-spacer
        v-if="resultValid"
      />

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
import { fixedImageName, movingImageName, matchName } from '@/models/constants/images';
import { matcherImageType } from '@/models/constants/matcherImageType';
import { paramTypes, ParamUtils } from '@/models/constants/params';

import defaultImage1 from '@/assets/zauberSchuleTemplate.jpg';
import defaultImage2 from '@/assets/zauberSchuleAlign.jpg';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppActionBar': ActionBar,
    'AppInputImages': InputImages,
    'AppImageResult': ImageResult
  },
  created() {
    this.$store.dispatch('input/init', {url1: defaultImage1, url2: defaultImage2});
    this.$store.dispatch('worker/load');
  },
  computed: {
    matcherImageTypes() {
      return matcherImageType;
    },
    matcherImageType() {
      return this.$store.getters['worker/results/matcherImageType'];
    },
    matchName() {
      return matchName;
    },
    resultValid() {
      return this.$store.getters['worker/results/imageDataValid'](matchName);
    },
    errorText() {
      const e = this.$store.getters['worker/error'];
      if(e && e.message) return e.message;
      return 'No match found!';
    },
    detTypeName() {
      return ParamUtils.getParamName(this.$store.getters['settings/param'](paramTypes.detType.id));
    },
    outputImageTypes() {
      return Object.values(this.matcherImageTypes);
    },
    blendSliderVisible() {
      return this.matcherImageType == this.matcherImageTypes.blended
          || this.matcherImageType == this.matcherImageTypes.stitched
          || this.matcherImageType == this.matcherImageTypes.stitchedWithKeyPoints
          || this.matcherImageType == this.matcherImageTypes.selectionImage1
          || this.matcherImageType == this.matcherImageTypes.selectionImage2;
    },
    pathPtsFixedImage() {
      return this.$store.getters['input/pathPtsFixedImage'];
    },
    pathPtsMovingImage() {
      return this.$store.getters['input/pathPtsMovingImage'];
    }
  },
  data() {
    return {
      blendValue: 0.5,
    }
  },
  watch: {
    pathPtsFixedImage() {
      if(this.matcherImageType == this.matcherImageTypes.selectionImage1) {
        this.requestImageIf();
      }
    },
    pathPtsMovingImage() {
      if(this.matcherImageType == this.matcherImageTypes.selectionImage2) {
        this.requestImageIf();
      }
    }
  },
  methods: {
    async match() {

      // eslint-disable-next-line no-console
      console.assert(
           this.$store.getters['input/imageDataValid'](fixedImageName)
        && this.$store.getters['input/imageDataValid'](movingImageName),
        "Invalid InputData in Matcher.match");

      await this.$store.dispatch(
        'worker/computeAlignedImage', {
          fixedImage: this.$store.getters['input/imageData'](fixedImageName),
          movingImage: this.$store.getters['input/imageData'](movingImageName),
          fixedImagePolygonPts: this.$store.getters['input/polygonClosedPts'](fixedImageName),
          movingImagePolygonPts: this.$store.getters['input/polygonClosedPts'](movingImageName),
          settings: this.$store.getters['settings/settings']
        }
      );
    },
    deleteResult() {
      this.$store.commit('worker/results/imageData', { name: matchName, imageData: null });
      this.$store.dispatch('worker/resetWorkerData');
    },
    loadDefaultImages() {
      this.$store.dispatch('input/loadDefaultImages', {url1: defaultImage1, url2: defaultImage2});
      this.$store.dispatch('worker/resetWorkerData');
    },
    imageSelectionChanged(value) {
      this.$store.commit('worker/results/matcherImageType', value);
      this.$store.dispatch('worker/requestImage');
    },
    blendValueChanged(value) {
      this.$store.commit('worker/results/matcherImageBlend', value);
      this.$store.dispatch('worker/requestImage');
    },
    addPolygonPt(name, pt) {
      const floodFillTolerance = this.$store.getters['settings/param'](paramTypes.floodFillTolerance.id);
      this.$store.commit('input/addPolygonPt', { name, pt, floodFillTolerance });
    },
    requestImageIf() {
      if(this.matcherImageType == this.matcherImageTypes.selectionImage1 ||
         this.matcherImageType == this.matcherImageTypes.selectionImage2) {
          
        this.$store.dispatch('worker/requestImage');
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