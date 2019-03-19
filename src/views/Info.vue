<template>
  <app-view-layout
    :title="'What does this application do?'"
    >
    <div>

      <p class="text-xs-left">
        A <strong>webapplication</strong> to <strong>join, stitch or align</strong> overlapping images.
        It can be used to create <strong>panoramas</strong> or to <strong>align images</strong>, for example forms which have not been scanned properly.
      </p>
      
      <p class="text-xs-left">
        The main technologies used are <a href="https://vuejs.org/"><strong>Vue</strong></a> and <a href="https://opencv.org/"><strong>OpenCV</strong></a>.
      </p>

      <p class="text-xs-left">
        <strong>Features:</strong> Surface projection, feature detection, feature matching, bundle adjustement, wave correction, color transfer, seam detection and multiband blending.
      </p>

      <p class="text-xs-left">
        The core algorithms are taken from OpenCV (modules
        <a href="https://github.com/opencv/opencv/tree/master/modules/features2d">Feature2D</a> and
        <a href="https://github.com/opencv/opencv/tree/master/modules/stitching">Stitching</a> mainly)
        and have been adjusted where needed. A customized version of OpenCV
        has then been compiled to <strong>wasm</strong>. To ensure a non blocking UI, all OpenCV related functions run via a <strong>webworker</strong>.
      </p>
      
      <p class="text-xs-left">
        <strong>Pros and cons:</strong> As image stitching can be quite <strong>memory and cpu intensive</strong>, there are some limitations
        on the amount or size of images which can be stitched together, especially on mobile devices.
        On the plus side, basic stitching functionality is available on almost any
        device which can run a browser. There is no need to download a professional stitching software.
        The application is <strong>fully functional offline</strong> as well.
      </p>

      <img
        :src="infoImage"
        alt="Info Stitching"
        width="100%"
      >

    </div>
  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';
import InfoImage from '@/assets/infoImage.jpg';

export default {
  components: {
    'AppViewLayout': ViewLayout
  },
  computed: {
    infoImage() {
      return InfoImage;
    }
  }
}
</script>

<style scoped>
  a {
    text-decoration: none;
  }
</style>
