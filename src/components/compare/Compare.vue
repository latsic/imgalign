<template>
  <v-layout>

    <v-layout
      row
      justify-space-around
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
        <app-select
          :param="param"
          @change="changed"
        />
      </v-flex>
    </v-layout>

  </v-layout>
</template>

<script>

import Select from '@/components/settings/Select';

export default {
  components: {
    'AppSelect': Select
  },
  props: {
    params: {
      type: Array,
      required: true
    }
  },
  methods: {
    changed(obj) {
      this.$emit('change', obj);
    }
  }
}
</script>

<style scoped>

</style>

