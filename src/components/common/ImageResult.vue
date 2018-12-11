<template>
  <div
    ref="mainDiv"
  >
    <img
      :style="{
        'padding': '0.3rem',
        border: `1px solid ${$vuetify.theme.primary}`,
        cursor: canShowDialog ? 'zoom-in' : null,
        width: 'auto',
        'max-width': '100%'
      }" 
      :id="imageName"
      :ref="imageName"
      :src="imageUrl"
      @click.stop="showUnscaledIf"
      @load="canShowDialog = isImageScaled()"
    >
    <v-dialog
      :attach="$refs.mainDiv"
      :ref="dialog.name"
      v-model="showDialog"
      persistent
      @keydown.esc="showDialog = false"
      :stlye="{
        cursor: 'move',
      }"
    >
      <img
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
    }
  },
  data() {
    return {
      showDialog: false,
      canShowDialog: false,
      htmlStyleOverflowY: null,

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

          if(clientX != this.startPos.x || clientY != this.startPos.y) {
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
        }
      }
    }
  },
  methods: {
    showUnscaledIf() {
      if(this.canShowDialog) {
        this.showDialog = true;
      }
    },
    isImageScaled() {
      return this.$refs
          && this.$refs[this.imageName]
          && (   this.$refs[this.imageName].naturalWidth > this.$refs[this.imageName].width
              || this.$refs[this.imageName].naturalHeight > this.$refs[this.imageName].height);
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
</style>
