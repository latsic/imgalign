<template>
  <app-view-layout
    :title="'What does this application do?'"
    >
    <div>
      <p class="text-xs-left">
        An spa to align an image to a template image. It uses a custom opencv version (compiled to wasm) to do so.
        To align the image 2d-feature detection is used (orb, sift, kaze, etc).
      </p>
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
