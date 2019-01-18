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
      xs6
      sm6
      pa-2
    >
      <v-switch
        v-if="param.type == valueTypes.bool"
        :value="param.value > 0 ? true : false"
        :input-value="param.value > 0 ? true : false"
        :label="paramName(param.id)"
        :style="{margin: '0', padding: '0'}"
        :disabled="isDisabled(param)"
        hide-details
        @change="value => changed(param.id, value ? 1 : 0 )"
      />
      <app-select
        v-if="param.type == valueTypes.discrete"
        :param="param"
        @change="changedDiscrete"
      />
    </v-flex>
    <v-flex
      xs6
      sm6
      pa-2
    >
      <v-text-field
        :name="'fieldOfView'"
        :label="'Field of view'"
        :hint="'Select input images'"
        persistent-hint
        :placeholder="fieldOfViewDefaultValue"
        :value="showFieldOfView ? fieldOfView : ''"
        :type="'number'"
        :disabled="!showFieldOfView"
        @change="value => fieldOfViewChanged(value)"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { valueTypes, ParamUtils } from '@/models/constants/params';
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
    showFieldOfView: {
      type: Boolean,
      default: false
    },
    fieldOfView: {
      type: String,
      default: null
    },
    fieldOfViewDefaultValue: {
      type: String,
      default: '45'
    }
  },
  data() {
    return {
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
    changedDiscrete(obj) {
      this.$emit('change', obj);
    },
    changed(id, value) {
      const valueNumber = Number(value);
      if(!isNaN(valueNumber)) {
        this.$emit('change', { id, value: valueNumber });
      }
    },
    fieldOfViewChanged(value) {
      this.$emit('fieldOfViewChange', value);
    },
    isDisabled(param) {

      const enabledIfId = ParamUtils.getParamEnabledIfId(param.id);
      if(!enabledIfId) return false;

      for(const p of this.params) {
        if(p.id == enabledIfId) {
          return p.value == 0;
        }
      }
      return false;
    },
  }
}
</script>

<style scoped>
</style>
