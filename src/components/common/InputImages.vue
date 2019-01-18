<template>
  <v-layout
    row
    justify-space-around
    wrap
    align-start
  >
    <v-flex
      v-for="imageName of imageNames"
      :key="imageName"
      xs12
      sm6
      pa-2
    > 
      <v-layout
        justify-center
      > 
        <div
          :style="{
            position: 'relative',
            border: $store.getters['input/imageDataValid'](imageName) ? `1px solid ${$vuetify.theme.primary}` : null,
            margin: '',
            padding: '0.3rem',
          }"
        >      
          <img
            :style="{
              cursor: 'crosshair',
              display: 'block',
              'max-width': '100%',
              'max-height': '400px',
              'width': 'auto',
              'height': 'auto'
            }"
            :class="{
              'drag-over-image-invalid': dragInfo.isInsideImage[imageName],
              'drag-over-image-maybevalid': dragInfo.mayContainImageData[imageName],
              'drag-over-image-valid': dragInfo.containsImageData[imageName],
            }"
            class="transparent-pattern"
            :id="imageName"
            :ref="imageName"
            
            :src="$store.getters['input/imageDataUrl'](imageName)"
            @click.capture="event => addPoint(event, imageName)"
            @mousedown.stop="(e) => {e.preventDefault()}"
            @dragenter.prevent="e => dragEnter(imageName, e)"
            @dragleave="e => dragLeave(imageName, e)"
            @dragover.prevent="e => dragOver(imageName, e)"
            @drop.prevent="e => dragDrop(imageName, e)"
          >
          <app-polygon
            :ref="imageName + 'polygon'"
            v-if="imageRef(imageName) && polygons[imageName] && polygons[imageName].pts.length > 0"
            :width="imageWidth(imageName)"
            :height="imageHeight(imageName)"
            :polygon="polygonScaled(imageName)"
          />
        </div>
      </v-layout>
    </v-flex>
  </v-layout>  
</template>

<script>

import Polygon from '@/components/common/Polygon';
import { DragEventInfo } from '@/utilities/DragEventInfo';
import { RemoteImage } from '@/utilities/RemoteImage';
import { fixedImageName } from '@/models/constants/images';

const imgIndex = 0;

export default {
  components: {
    'AppPolygon': Polygon
  },
  props: {
    polygons: {
      type: Object,
      required: true
    }
  },
  data() {
    return {
      polysNeedUpdate: {},
      dragInfo: {
        isInsideImage: {},
        mayContainImageData: {},
        containsImageData: {}
      }
    }
  },
  computed: {
    imageNames() {
      return this.$store.getters['input/allImageNames'];
    }
  },
  methods: {
    poylgonRefName(imageName) {
      return imageName + 'polygon';
    },
    imageRef(imageName) {
      if(!this.$refs[imageName]) return null;
      return this.$refs[imageName][imgIndex];
    },
    polyRef(imageName) {
      if(!this.$refs[this.poylgonRefName(imageName)]) return null;
      return this.$refs[this.poylgonRefName(imageName)][imgIndex];
    },
    addPoint(event, imageName) {
      
      if(!this.imageRef(imageName)) return;

      const pt = { x: event.offsetX, y : event.offsetY };
      const scale = this.imageRef(imageName).naturalWidth / this.imageRef(imageName).width;
      this.$emit('addPolygonPoint', imageName, { x: pt.x * scale, y: pt.y * scale });

      this.polysNeedUpdate[imageName] = true;
    },
    imageWidth(imageName) {
      return this.imageRef(imageName) && this.imageRef(imageName).width
        ? this.imageRef(imageName).width
        : 0
    },
    imageHeight(imageName) {
      return this.imageRef(imageName) && this.imageRef(imageName).height
        ? this.imageRef(imageName).height
        : 0
    },
    polygonScaled(imageName) {
      if(!this.imageRef(imageName)) return null;
      if(!this.polygons[imageName]) return null;

      return this.polygons[imageName].scaledCopy(
        this.imageRef(imageName).width / this.imageRef(imageName).naturalWidth);
    },
    dragEnter(imageName, e) {
      this.dragInfo.isInsideImage[imageName] = true;
      this.dragInfo.mayContainImageData[imageName] = new DragEventInfo(e).mayContainImageData;
      this.dragInfo.containsImageData[imageName] = new DragEventInfo(e).containsImageData;
    },
    dragLeave(imageName) {
      this.dragInfo.isInsideImage[imageName] = false;
      this.dragInfo.mayContainImageData[imageName] = false;
      this.dragInfo.containsImageData[imageName] = false;
    },
    dragOver() {
    },
    async dragDrop(imageName, e) {
      this.dragInfo.isInsideImage[imageName] = false;
      this.dragInfo.mayContainImageData[imageName] = false;
      this.dragInfo.containsImageData[imageName] = false;
      const dragEventInfo = new DragEventInfo(e);
      
      const imageFile = dragEventInfo.imageFile;
      if(imageFile) {
        this.$store.dispatch('input/imageFile', {
          name: imageName,
          file: imageFile
        });
      }
      else {
        const imageUrlBase64 = dragEventInfo.imageUrlBase64;
        if(imageUrlBase64 && (imageName == fixedImageName || !dragEventInfo.projected)) {
          this.$store.dispatch('input/imageUrlBase64', {
            name: imageName,
            url: imageUrlBase64,
            fieldOfView: dragEventInfo.fieldOfView,
            projected: dragEventInfo.projected
          });
        }
        else {
          const imageUrlRemote = dragEventInfo.imageUrlRemote;
          if(imageUrlRemote) {
            const remoteImage = new RemoteImage(imageUrlRemote);
            try {
              const imageBase64 = await remoteImage.imageBase64;
              this.$store.dispatch('input/imageUrlBase64', {
                name: imageName,
                url: imageBase64
              });
              remoteImage.imageBase64
            }
            catch(error) {
              // eslint-disable-next-line no-console
              console.log('[InputImages][dragDrop][Error]', error);
            }
          }
        }
      }
    }
  },
  updated() {

    for(const imageName of this.imageNames) {
      if(this.polyRef(imageName) && this.polysNeedUpdate[imageName]) {
        this.polyRef(imageName).$forceUpdate();
        this.polysNeedUpdate[imageName] = false;
      }
    }
  },
  created() {
    for(const imageName of this.imageNames) {
      this.$set(this.polysNeedUpdate, imageName, true);
      this.$set(this.dragInfo.isInsideImage, imageName, false);
      this.$set(this.dragInfo.mayContainImageData, imageName, false);
      this.$set(this.dragInfo.containsImageData, imageName, false);
    }
  },
  mounted() {
    if(window && !this.resizeListener) {
      this.resizeListener = window.addEventListener('resize', () => {
        for(const imageName of this.imageNames) {
          this.polysNeedUpdate[imageName] = true;
          this.$forceUpdate();
        }
      });
    }
    for(const imageName of this.imageNames) {
      if(this.polysNeedUpdate[imageName]) this.$forceUpdate();
    }
  },
  beforeDestroy() {
    if(window && this.resizeListener) {
      window.removeEventListener(this.resizeListener);
      this.resizeListener = null;
    }
  }
  
  
}
</script>

<style scope>
  .drag-over-image-invalid {
    outline: 2px solid red;
  }
  .drag-over-image-maybevalid {
    outline: 2px solid orange;
  }
  .drag-over-image-valid {
    outline: 2px solid green;
  }
  .transparent-pattern {
    background: 
      repeating-linear-gradient(
        to right,
        #ccccff99,
        #ccccff99 5px,
        #eeeeff99 5px,
        #eeeeff99 10px
      ),
      repeating-linear-gradient(
        to bottom,
        #cccc88ff,
        #cccc88ff 5px,
        #999966ff 5px,
        #999966ff 10px
      );

  }
  
</style>
