<template>

  <!-- <transition-group -->
  <div
    tag="div"
    :name="doTransition ? '' : ''"

    :style="{
      display: 'grid',
      'grid-gap': '0px',
      'grid-template-columns': `repeat(auto-fill, ${gridCellWidth}%)`,
      'grid-template-rows': 'auto',
      'margin': '0 auto',
      'margin-top': '1rem',
      'width': gridWidth + '%',
    }"
  >
    
    <div
      class="list-item"
      v-for="(imageUrl, index) of imageUrlArray"
      :key="imageKeyArray[index]"
    
      :style="{
        outline: $store.getters['multiInput/imageDataValid'](index)
          ? `${borderLineWidth(index)} solid ${borderColor(index)}`
          : null,
        margin: '3px',
        display: 'flex',
        'align-items': 'center'
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
  </div>
  <!-- </transition-group> -->
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
  data() {
    return {
      currentGridWidth: this.gridWidth,
      leaveOngoing: false
    }
  },
  computed: {
    gridCellWidth() {

      if(this.imageUrlArray.length <= 1) {
        return 100;
      }
      else if(this.imageUrlArray.length == 2 || this.$vuetify.breakpoint.name == 'xs') {
        return 50;
      }
      else if(this.imageUrlArray.length == 3 || this.$vuetify.breakpoint.name == 'sm') {
        return 33.333333;
      }
      else {
        return 25;
      }
    },
    gridWidth() {
      
      let w;

      if(this.$vuetify.breakpoint.name == 'xs') {
        w = this.imageUrlArray.length * 50;
      }
      else if(this.$vuetify.breakpoint.name == 'sm') {
        w = this.imageUrlArray.length * 33.3333333;
      }
      else {
        w = this.imageUrlArray.length * 25;
      }
      // console.log("gridW: ", w);
      return w >= 100 ? 100 : w;
    },
    doTransition() {
      if(this.$vuetify.breakpoint.name == 'xs') {
        return this.imageUrlArray.length > 2 - 1
      }
      else if(this.$vuetify.breakpoint.name == 'sm') {
        return this.imageUrlArray.length > 3 - 1
      }
      else {
        return this.imageUrlArray.length > 4 - 1
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
    },
    gridCellWidth2() {

      if(this.imageUrlArray.length <= 1) {
        return 100;
      }
      else if(this.imageUrlArray.length == 2 || this.$vuetify.breakpoint.name == 'xs') {
        return 50;
      }
      else if(this.imageUrlArray.length == 3 || this.$vuetify.breakpoint.name == 'sm') {
        return 33.333333;
      }
      else {
        return 25;
      }
    },
    gridWidth2() {
      
      let w;

      if(this.$vuetify.breakpoint.name == 'xs') {
        w = this.imageUrlArray.length * 50;
      }
      else if(this.$vuetify.breakpoint.name == 'sm') {
        w = this.imageUrlArray.length * 33.3333333;
      }
      else {
        w = this.imageUrlArray.length * 25;
      }
      // console.log("gridW: ", w);
      return w >= 100 ? 100 : w;
    },
  }
}
</script>

<style scoped>

.list-item {
  
}
.list-enter{
  opacity: 0;
}
.list-enter-active{
  transition: opacity 1s;
}
.list-leave {
  visibility: hidden;
}
.list-leave-active {
  position: absolute;
  visibility: hidden;
}
.list-move {
  transition: all 3s;
}



</style>
