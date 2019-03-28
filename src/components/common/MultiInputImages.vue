<template>

  <transition
    name="images-all"
  >
    <div
      v-if="imageUrlArray.length > 0"
      :style="{
        'width': '100%',
        'border': '1px solid #00000055',
        'background-color': '#f6f6f6',
        'padding': '0.1rem 0.1rem',
        'margin-top': '1rem',
      }"
    >
    
      <transition-group
        tag="div"
        :name="'images-list-items'"
        :style="{
          display: 'grid',
          'grid-template-columns': `repeat(auto-fill, ${gridCellWidth}%)`,
          'grid-template-rows': 'auto',
          'margin': '0 auto',
          width: '100%'
        }"
      >
        
        <div
          v-for="(imageUrl, index) of imageUrlArray"
          :key="imageKeyArray[index]"
        
          :style="{
            outline: $store.getters['multiInput/imageDataValid'](index)
              ? `${borderLineWidth(index)} solid ${borderColor(index)}`
              : null,
            margin: '3px',
            display: 'flex',
            'align-items': 'center',
            'justify-content': 'center'
          }"
          @click.stop="imageClicked(index)"
          @drop.prevent="e => dragDrop(e, index)"
          @dragover.prevent="() => {}"
          @dragend.prevent="() => {}"
          @dragenter.prevent="() => {}"
        >
          <div
            :style="{
              padding: '0.3rem',
              'background-color': '#ffffff'
            }"
          > 
            <div
              :style="{
                position: 'relative'
              }"
              >    
              <img
                :style="{
                  display: 'block',
                  'max-width': '100%',
                  'max-height': '100%',
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
                  padding: '0.1rem 0.3rem',
                  margin: '0 0.2rem 0.2rem 0'
                }"
              >
                <strong>{{ index }}</strong>
              </div>
            </div> 
          </div>
            
        
        </div>
      </transition-group>
    </div>
    </transition>
</template>

<script>
export default {
  props: {
    imageUrlArray: {
      type: Array,
      required: true
    },
    imageKeyArray: {
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
    gridCellWidth() {
      if(this.$vuetify.breakpoint.name == 'xs') {
        return 50;
      }
      else if(this.$vuetify.breakpoint.name == 'sm') {
        return 33.3333333;
      }
      else {
        return 25;
      }
    },
    // gridWidth() {
    //   let w;
    //   if(this.$vuetify.breakpoint.name == 'xs') {
    //     w = this.imageUrlArray.length * 50;
    //   }
    //   else if(this.$vuetify.breakpoint.name == 'sm') {
    //     w = this.imageUrlArray.length * 33.3333333;
    //   }
    //   else {
    //     w = this.imageUrlArray.length * 25;
    //   }
    //   return w >= 100 ? 100 : w;
    // }
  },
  methods: {
    imageClicked(index) {
      this.$emit('imageClicked', index);
    },
    borderColor(index) {
      return this.indicesSelected.some(selectedIndex => selectedIndex == index)
        ? this.$vuetify.theme.error
        : this.$vuetify.theme.secondary;
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

  .images-list-items-enter{
    opacity: 0;
  }
  .images-list-items-active{
    transition: opacity 0.7s;
  }
  .images-list-items-leave {
    visibility: hidden;
  }
  .images-list-items-leave-active {
    position: absolute;
    visibility: hidden;
  }
  .images-list-items-move {
    transition: all 1s;
  }

  .images-all-leave-active,
  .images-all-enter-active {
    transition: opacity 1s;
  }
  .images-all-leave-to,
  .images-all-enter {
    opacity: 0;
  }

</style>