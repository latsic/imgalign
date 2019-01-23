<template>
  <div
    ref="mainDiv"
  >
    <div
      :style="{
        border: `1px solid ${$vuetify.theme.primary}`,
        'padding': '0.3rem',
      }"
    >
      <img
        :style="{
          display: 'block',
          'max-width': '100%',
          'max-height': '50vh',
          'width': 'auto',
          'height': 'auto',
          'margin': 'auto',
          'padding': '0'
        }"
        :class="{'cursor-drag-zoom': canShowDialog, 'cursor-drag': !canShowDialog}"
        class="transparent-pattern"
        :id="imageName"
        :ref="imageName"
        :src="imageUrl"
        draggable="true"
        @click.stop="showUnscaledIf"
        @load="canShowDialog = isImageScaled()"
        @dragstart="dragStart"
        @dragover.prevent="() => {}"
        @dragend.prevent="() => {}"
      >
    </div>
    <v-dialog
      :attach="$refs.mainDiv"
      :ref="dialog.name"
      v-model="showDialog"
      persistent
      @keydown.esc="showDialog = false"
      :stlye="{
        cursor: 'move',
      }"
      content-class="transparent-pattern"
    >
      <img
        class="transparent-pattern"
        :id="dialog.imageName"
        :ref="dialog.imageName"
        :src="imageUrl"
        :stlye="{
          cursor: 'move',
          display: 'block'
        }"
        @mousedown.left.stop="startDrag"
        @touchstart.stop="startDrag"
      >
    </v-dialog>
  </div>
</template>

<script>

export default {
  props: {
    imageName: {
      type: String,
      required: true
    },
    imageUrl: {
      validator() {
        return true;
      },
      required: true
    },
    projected: {
      type: Boolean,
      default: false
    },
    fieldOfView: {
      type: Number,
      default: 0
    }
  },
  data() {
    return {
      showDialog: false,
      canShowDialog: false,
      htmlStyleOverflowY: null,
      clickPos: {x: null, y: null },

      dialog: {
        name: 'dialog_000a',
        imageName: this.imageName + 'D',

        startPos: {x: null, y: null },
        dims: { w: null, h: null },
        scrollInfo: {
          w: null, h: null,
          lMax: null, lStart: null,
          tMax: null, tStart: null
        },
        
        innerDialog: function(refs) { return refs[this.name].$refs.dialog },
        hideScrollbars: function(refs) {
          if(this.innerDialog(refs).style.overflow != 'hidden') { // ???
            this.innerDialog(refs).style.overflow = 'hidden';
          }
        },
        setMoveCursor: function(refs) {
          this.innerDialog(refs).style.cursor = 'move';
        },
        dragMode: function() { return this.startPos.x != null },
        stop: function()  { this.startPos.x = null },
        start: function(clientX, clientY, refs) {
          
          // eslint-disable-next-line no-console
          console.assert(this.innerDialog(refs), 'Inner dialog must not be null');

          this.startPos.x = clientX;
          this.startPos.y = clientY;
          this.dims.w = this.innerDialog(refs).clientWidth;
          this.dims.h = this.innerDialog(refs).clientHeight;

          this.scrollInfo.w = refs[this.imageName].scrollWidth;
          this.scrollInfo.h = refs[this.imageName].scrollHeight;
          
          this.scrollInfo.lMax = this.scrollInfo.w - this.dims.w;
          this.scrollInfo.tMax = this.scrollInfo.h - this.dims.h;

          this.scrollInfo.lStart = this.innerDialog(refs).scrollLeft;
          this.scrollInfo.tStart = this.innerDialog(refs).scrollTop;

          this.moved = false;
        },
        move: function(clientX, clientY, refs) {
          if(!this.dragMode()) return;

          if(  Math.abs(this.startPos.x - clientX) > 2
            || Math.abs(this.startPos.y - clientY) > 2) {

            this.moved = true;
          }

          const sL = this.scrollInfo.lStart + this.startPos.x - clientX;
          if(sL < this.scrollInfo.lMax) {
            this.innerDialog(refs).scrollLeft = sL;
          }

          const sT = this.scrollInfo.tStart + this.startPos.y - clientY;
          if(sT < this.scrollInfo.tMax) {
            this.innerDialog(refs).scrollTop = sT;
          }
        },
        init: function(x, y, refs) {

          const dimW = this.innerDialog(refs).clientWidth;
          const dimH = this.innerDialog(refs).clientHeight;

          const scrollInfoW = refs[this.imageName].scrollWidth;
          const scrollInfoH = refs[this.imageName].scrollHeight;
          
          const scrollInfoLMax = scrollInfoW - dimW;
          const scrollInfoTMax = scrollInfoH - dimH;

          const sL = x - dimW / 2;
          const sT = y - dimH / 2;

          this.innerDialog(refs).scrollLeft = sL < scrollInfoLMax
            ? sL
            : scrollInfoLMax;

          this.innerDialog(refs).scrollTop = sT < scrollInfoTMax
            ? sT
            : scrollInfoTMax;

        }
      }
    }
  },
  methods: {
    showUnscaledIf(event) {
      if(this.canShowDialog) {
        this.showDialog = true;
        
        const scaleFactor = this.scaleFactor();
        if(scaleFactor > 1) {
          this.clickPos.x = scaleFactor * (event.layerX - event.target.offsetLeft);
          this.clickPos.y = scaleFactor * (event.layerY - event.target.offsetTop);
        }
      }
    },
    isImageScaled() {
      return this.$refs
          && this.$refs[this.imageName]
          && (   this.$refs[this.imageName].naturalWidth > this.$refs[this.imageName].width
              || this.$refs[this.imageName].naturalHeight > this.$refs[this.imageName].height);
    },
    scaleFactor() {
      if(!this.isImageScaled()) {
        return 1.0;
      }
      return this.$refs[this.imageName].naturalWidth / this.$refs[this.imageName].width;
    },
    startDrag(event) {
      event.preventDefault();
      this.dialog.start(...this.coords(event), this.$refs);
      this.addListeners();
    },
    endDrag(){
      this.removeAllDialogListeners();
      if(this.dialog.dragMode() && !this.dialog.moved) {
        this.showDialog = false;
      }
      this.dialog.stop();
    },
    addListeners() {
      if(!window) return;
      if(!this.mousemoveListener) {
        this.mousemoveListener = window.addEventListener('mousemove',
          event => this.dialog.move(...this.coords(event), this.$refs));
      }
      if(!this.touchmoveListener) {
        this.touchmoveListener = window.addEventListener('touchmove',
          event => this.dialog.move(...this.coords(event), this.$refs));
      }
      if(!this.mouseupListener) {
        this.mouseupListener = window.addEventListener('mouseup', this.endDrag);
      }
      if(!this.touchendListener) {
        this.touchendListener = window.addEventListener('touchend', this.endDrag);
      }
    },
    removeAllDialogListeners() {
      if(!window) return;
      if(this.mousemoveListener) {
        window.removeEventListener(this.mousemoveListener);
        this.mousemoveListener = null;
      }
      if(this.mouseupListener) {
        window.removeEventListener(this.mouseupListener);
        this.mouseupListener = null;
      }
      if(this.touchmoveListener) {
        window.removeEventListener(this.touchmoveListener);
        this.mousemoveListener = null;
      }
      if(this.touchendListener) {
        window.removeEventListener(this.touchendListener);
        this.touchendListener = null;
      }
    },
    coords(event) {
      if(event.touches && event.touches.length > 0) {
        return [event.touches[0].clientX, event.touches[0].clientY];
      }
      else {
        return [event.clientX, event.clientY];
      }
    },
    dragStart(event) {
      event.dataTransfer.setData('text/plain', this.imageUrl);
      event.dataTransfer.setData('number/projected', this.projected ? 1 : 0);
      event.dataTransfer.setData('number/fieldOfView', this.fieldOfView);
    }
  },
  mounted() {
    if(window && !this.resizeListener) {
      this.resizeListener = window.addEventListener('resize', () => {
        this.canShowDialog = this.isImageScaled();
        if(!this.canShowDialog && this.showDialog) {
          this.showDialog = false;
        }
      });
    }
  },
  beforeUpdate() {
    
    // A workaround to not remove the scrollbar (if any) of the page if
    // the dialog is shown. Otherwise the page jumps around when
    // the dialog is shown or removed.

    if(this.showDialog) {
      this.htmlStyleOverflowY = document.documentElement.style.overflowY;
      document.documentElement.style.overflowY = "auto"
    }
    else {
      document.documentElement.style.overflowY = this.htmlStyleOverflowY;
      if(document.documentElement.classList.contains('overflow-y-hidden')) {
        document.documentElement.classList.remove('overflow-y-hidden');
      }
    }
  },
  updated() {
    if(this.showDialog) {
      this.dialog.hideScrollbars(this.$refs);
      this.dialog.setMoveCursor(this.$refs);

      if(this.clickPos.x != null && this.clickPos.y != null) {
        this.dialog.init(this.clickPos.x, this.clickPos.y, this.$refs);
        this.clickPos.x = null;
        this.clickPos.y = null;
      }
    }
  },
  beforeDestroy() {
    this.removeAllDialogListeners();
    if(window && this.resizeListener) {
      window.removeEventListener(this.resizeListener);
      this.resizeListener = null;
    }
  }
}
</script>

<style scoped>
  .overflow-y-hidden {
    overflow-y: auto !important;
  }
  #app {
    overflow-y: auto !important;
  }
  .cursor-drag-zoom {
    cursor: url(../../assets/zoom-drag-24.png) 2 2, auto;
  }
  .cursor-drag {
    cursor: move;
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
