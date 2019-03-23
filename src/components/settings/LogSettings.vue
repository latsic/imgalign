<template>
  <v-layout
    row
    justify-start
    wrap
    align-baseline
  >
    <v-flex
      v-for="param of params"
      :key="param.id"
      xs12
      sm6
    >
      <v-switch
        v-if="param.type == valueTypes.bool"
        :value="switchValue(param)"
        :input-value="switchValue(param)"
        :label="paramName(param.id)"
        :hint="paramText(param.id)"
        @change="value => $emit('changed', { id: param.id, value: value ? 1 : 0 })"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { ParamUtils, valueTypes } from '@/models/constants/params';

export default {
  props: {
    params: {
      type: Array,
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
    switchValue(param) {
      return param.value > 0 ? true : false;
    }
  }
}
</script>

<style scoped>

</style>
