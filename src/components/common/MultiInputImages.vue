<template>
  <v-layout
    row
    justify-space-around
    wrap
    @click="imageClicked(-1)"
  >
    <v-flex
      v-for="(imageUrl, index) of imageUrlArray"
      :key="index"
      v-bind="layoutProps"
      pa-1
      >
      <v-layout
        row
        fill-height
        align-center
        justify-center
        :style="{
          border: $store.getters['multiInput/imageDataValid'](index)
            ? `${borderLineWidth(index)} solid ${borderColor(index)}`
            : null,
        }"
        @click.stop="imageClicked(index)"
       
        @drop.prevent="e => dragDrop(e, index)"
        @dragover.prevent="() => {}"
        @dragend.prevent="() => {}"
        @dragenter.prevent="() => {}"
        >
        
          <div
            :style="{
              position: 'relative',
              padding: '0.3rem',
            }"
          > 
            <div
              :style="{
                position: 'relative',
              }"
              >    
              <img
                :style="{
                  display: 'block',
                  'max-width': '100%',
                  'max-height': '30vh',
                  'width': 'auto',
                  'height': 'auto'
                }"
                class="transparent-pattern"
                :id="index + ''"
                :ref="index + ''"
                :src="imageUrl" 
                @dragstart="e => dragStart(e, index)"
                @dragover.prevent="() => {}"
                @dragend.prevent="() => {}"
              >
            
              <div
                :style="{
                  position: 'absolute',
                  bottom: '0px',
                  right: '0px',
                  color: $vuetify.theme.primary,
                  transform: 'translateY(0%) translateX(0%)',
                  'background-color': $vuetify.theme.accent,
                  padding: '0.1rem 0.3rem'
                }"
              >
                <strong>{{ index }}</strong>
              </div>
            </div> 
          </div>
         
      </v-layout>
    </v-flex>
    </v-layout>
    
</template>

<script>
export default {
  props: {
    imageUrlArray: {
      type: Array,
      required: true
    },
    fieldOfViewArray: {
      type: Array,
      required: true
    },
    indicesSelected: {
      type: Array,
      required: true
    }
  },
  computed: {
    layoutProps() {

      if(this.imageUrlArray.length <= 1) {
        return {
          xs12: true,
        }
      }
      else if(this.imageUrlArray.length == 2) {
        return {
          xs6: true,
        }
      }
      else if(this.imageUrlArray.length == 3) {
        return {
          xs4: true,
        }
      }
      return {
        xs4: true,
        sm3: true,
        md3: true
      }
    }
  },
  methods: {
    imageClicked(index) {
      this.$emit('imageClicked', index);
    },
    borderColor(index) {
      return this.indicesSelected.some(selectedIndex => selectedIndex == index)
        ? this.$vuetify.theme.error
        : this.$vuetify.theme.primary;
    },
    borderLineWidth(index) {
      return this.indicesSelected.some(selectedIndex => selectedIndex == index)
        ? '2px'
        : '1px';
    },
    dragStart(event, index) {
      event.dataTransfer.setData('number/indexFrom', index);
    },
    dragDrop(event, index) {
      let indexFrom = event.dataTransfer.getData('number/indexFrom');
      if(indexFrom) indexFrom = Number(indexFrom);
      if(!isNaN(indexFrom)) {
        this.$emit('swap', {indexFrom, indexTo: index });
      }
    }
  }
}
</script>

<style scoped>
 
</style>
