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
        class="switch-text-small"
      />
      <app-select
        v-if="param.type == valueTypes.discrete"
        :param="param"
        @change="changedDiscrete"
      />
      <v-slider
        v-if="param.type == valueTypes.range"  
        always-dirty
        v-bind="additionalSliderAttributes(param)"
        :value="param.value"
        :max="param.range.max"
        :min="param.range.min"
        :step="0.01"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        :disabled="isDisabled(param)"
        @change="value => $emit('change', { id: param.id, value })"
      />
    </v-flex>
    <v-flex
      xs6
      sm6
      pa-2
      v-if="showFieldOfView"
    >
      <v-text-field
        :name="'fieldOfView'"
        :label="'Field of view'"
        :hint="'Select input images'"
        persistent-hint
        :placeholder="fieldOfViewDefaultValue"
        :value="showFieldOfView && enableFieldOfView ? fieldOfView : ''"
        :type="'number'"
        :disabled="!enableFieldOfView"
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
    enableFieldOfView: {
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
    additionalSliderAttributes(param) {
      if(this.$vuetify.breakpoint.name == "xs") {
        return {
          hint: `${this.paramName(param.id)} ${param.value}`,
          'persistent-hint': true,
          label: '',
          'hide-details': false
        }
      }
      else {
        return {
          label: `${this.paramName(param.id)} ${param.value}`,
          'persistent-hint': false,
          hint: null,
          'hide-details': true
        }
      }
    }
  }
}
</script>

<style scoped>
  .switch-text-small >>> label {
    font-size: 0.7em;
    font-weight: 600; 
  }
</style>
