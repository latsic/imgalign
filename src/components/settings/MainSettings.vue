<template>
  <v-layout
    row
    justify-start
    wrap
    align-baseline
  >
    
    <v-flex
      xs12
    >
      <v-layout
        row
      >
        <v-flex
          xs6
          pa-2
        >
          <app-select
            v-if="paramsOpenCv.length > 0 && paramsOpenCv[0].type == valueTypes.discrete"
            :param="paramsOpenCv[0]"
            @change="value => $emit('changed', value)"
          />
        </v-flex>
        <v-flex
          xs6
          pa-2
          justify-end
        >
          <v-layout
            justify-end
          >
          <v-btn
            v-if="paramsOpenCv.length > 0 && paramsOpenCv[0].type == valueTypes.discrete"
            :style="{
              'text-transform': 'none !important',
            }"
            :color="'primary'"
            :disabled="openCvLoading"
            small
            @click="$emit('load-opencv')"
          >
            {{ openCvReady ? 'Reload' : 'Load' }}
          </v-btn>
          </v-layout>
        </v-flex>
      </v-layout>
    </v-flex>


    <v-flex
      v-for="param of params"
      :key="param.id"
      xs12
      sm6
      pa-2
    >
      <v-slider
        v-if="param.type == valueTypes.rangeSquareRoot"  
        always-dirty
        :value="sliderValueSqrt(param.value)"
        :hint="`${paramName(param.id)} ${param.value} px`"
        persistent-hint
        :max="sliderValueSqrt(param.range.max)"
        :min="sliderValueSqrt(param.range.min)"
        :step="20"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        @change="value => $emit('changed', { id: param.id, value: value * value })"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { ParamUtils, valueTypes } from '@/models/constants/params';
import Select from '@/components/settings/Select';

export default {
  components: {
    'AppSelect': Select
  },
  props: {
    params: {
      type: Array,
      required: true
    },
    paramsOpenCv: {
      type: Array,
      required: true
    },
    openCvReady: {
      type: Boolean,
      required: true
    },
    openCvLoading: {
      type: Boolean,
      required: true
    }
  },
  computed: {
    valueTypes() {
      return valueTypes;
    }
  },
  methods: {
    paramName(id) {
      return ParamUtils.getParamName(id);
    },
    paramText(id) {
      return ParamUtils.getParamText(id);
    },
    sliderValueSqrt(value) {
      return Math.round(Math.sqrt(value));
    }
  }
}
</script>

<style scoped>

</style>
