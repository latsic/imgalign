<template>
  <app-view-layout
    :title="'Lates log messages'"
  >
    <div
      v-for="(msg, index) of $store.getters['logs/messages'].slice().reverse()"
      :v-text="msg"
      :key="index"
      :style="{
        margin: '0',
        padding: '0',
        'font-size': '0.7em',
        'text-align': 'left',
        color: index % 2 == 0 ? $vuetify.theme.secondary : $vuetify.theme.primary }"
      >
      {{ msg }}
    </div>
  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';

export default {
  components: {
    'AppViewLayout': ViewLayout
  }
}
</script>

<style scoped>

</style>
