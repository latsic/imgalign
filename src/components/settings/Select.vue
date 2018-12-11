<template>
  <v-select
    v-if="canRender"
    :placeholder="paramName()"
    :items="paramMapped()"
    :value="paramValue()"
    :hint="paramName()"
    persistent-hint
    single-line
    @change="changed"
    v-bind="additionalSettings()"
    :style="{margin: '0', padding: '0'}"
    :item-text="'text'"
    :item-value="'value'"
  >
    <template
      slot="selection"
      slot-scope="{ item, index }"
    >
      <span
        v-if="index === 0"
      >
        {{ item.text }}
      </span>
      <span
        v-else-if="index === 1"
      >
        , {{ item.text }}
      </span>
      <span
        v-else-if="paramValue().length === 3"
        class="caption"
      >
        &nbsp;( +1 other )
      </span>
      <span
        v-else-if="index === paramValue().length - 1"
        class="caption"
      >
        &nbsp;, ( + {{ paramValue().length - 2 }}  )
      </span>
    </template>
  </v-select>

</template>

<script>

import { valueTypes, ParamUtils } from '@/models/constants/params';

export default {
  props: {
    param: {
      type: Object,
      required: true
    }
  },
  computed: {
    canRender() {
      return this.param.type == valueTypes.discrete
          || this.param.type == valueTypes.multiple;
    }
  },
  methods: {
    paramMapped() {
      
      return this.param.values.map(param => {
        return { text: param.name, value: param.id };
      });
    },
    changed(newValue) {
      if(this.param.type == valueTypes.multiple) {
        this.$emit('change', { id: this.param.id, value: newValue.toString() });
      }
      else {
        this.$emit('change', { id: this.param.id, value: newValue });
      }
    },
    paramName() {
      return ParamUtils.getParamName(this.param.id);
    },
    paramValue() {
      if(this.param.type == valueTypes.discrete) {
        return { text: ParamUtils.getParamName(this.param.value), value: this.param.value };
      }
      return this.paramValueMultiple();
    },
    paramValueMultiple() {

      if(this.param.value.length == 0) return [];

      const paramValues = this.param.value.length > 1
        ? this.param.value.split(',')
        : [this.param.value];

      return paramValues.map(paramValue => {
        return {
          text: ParamUtils.getParamName(+paramValue),
          value: +paramValue
        }
      });
    },
    additionalSettings() {
      return {
        multiple: this.param.type == valueTypes.multiple
      }
    }

  }
}
</script>

<style scoped>

</style>
