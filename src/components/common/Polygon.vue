<template>
  <canvas
    ref="canvas"
    :width="width"
    :height="height"
    :style="{
      position: 'absolute',
      transform: 'translate(-50%, -100%)',
      padding: padding,
      'pointer-events': 'none'
    }"
  />
</template>

<script>

import { style } from '@/models/constants/polygon';

export default {
  props: {
    polygon: {
      type: Object,
      required: true
    },
    width: {
      type: Number,
      required: true
    },
    height: {
      type: Number,
      required: true
    },
    padding: {
      type: String,
      default: '0'
    }
  },
  mounted() {
    this.clearRect({
      tl: { x: 0, y: 0 },
      w: this.width,
      h: this.height
    });
    this.drawPath();
  },
  beforeUpdate() {
    if(this.lastBoundingBox) {
      this.clearRect(this.lastBoundingBox);
    }
    this.lastBoundingBox = this.bounds();
    this.drawPath();
    if(!this.polygon.isClosed()) {
      this.drawPts();
    }
  },
  computed: {
    pts() {
      if(!this.polygon) return [];
      return this.polygon.pts;
    }
  },
  data() {
    return {
      lastBoundingBox: null
    }
  },
  methods: {
    ctx() {
      if(!this.$refs['canvas'].getContext) return;
      return this.$refs['canvas'].getContext('2d');
    },
    clearRect(box) {
      if(box) {
        const ctx = this.ctx();
        if(!ctx) return;

        ctx.clearRect(box.tl.x, box.tl.y, box.w, box.h);
      }
    },
    drawPts() {
      const ctx = this.ctx();
      if(!ctx) return;

      for(let i = 0; i < this.pts.length; ++i) {
        const color1 = i == 0 ? style.ptStartColor1 : style.ptColor1;
        const color2 = i == 0 ? style.ptStartColor2 : style.ptColor2;

        ctx.fillStyle = color1;
        ctx.fillRect(
          this.pts[i].x - style.ptRadius1,
          this.pts[i].y - style.ptRadius1,
          style.ptRadius1 * 2, style.ptRadius1 * 2);
        ctx.fillStyle = color2;
        ctx.fillRect(
          this.pts[i].x - style.ptRadius2,
          this.pts[i].y - style.ptRadius2,
          style.ptRadius2 * 2, style.ptRadius2 * 2);
      }
    },
    drawPath() {
      if(this.pts.length < 2) return;

      const ctx = this.ctx();
      if(!ctx) return;

      const isClosed = this.polygon.isClosed();
      
      ctx.strokeStyle = style.pathColor;
      ctx.lineWidth = style.pathWidth;
      ctx.beginPath();

      ctx.moveTo(this.pts[0].x, this.pts[0].y);
      for(let i = 1; i < this.pts.length; ++i) {
        ctx.lineTo(this.pts[i].x, this.pts[i].y);
        
        ctx.strokeStyle = style.pathColor1;
        ctx.lineWidth = style.pathWidth1;
        ctx.stroke();

        ctx.strokeStyle = style.pathColor2;
        ctx.lineWidth = style.pathWidth2;
        ctx.stroke();
      }
      if(isClosed) {
        ctx.fillStyle = style.fillColor;
        ctx.fill();
      }
    },
    bounds() {
      
      if(this.pts.length == 0) {
        return { tl: { x: 0, y: 0 }, w: 0, h: 0 };
      }

      const { tl, br } = this.polygon.bbox;

      tl.x = tl.x - style.ptRadius1 >= 0 ? tl.x - style.ptRadius1 : 0;
      tl.y = tl.y - style.ptRadius1 >= 0 ? tl.y - style.ptRadius1 : 0;
      br.x = br.x + style.ptRadius1 <= this.width ? br.x + style.ptRadius1 : this.width;
      br.y = br.y + style.ptRadius1 <= this.height ? br.y + style.ptRadius1 : this.height;

      return { tl, w: br.x - tl.x, h: br.y - tl.y };
    }
  }
}
</script>

<style scoped>

</style>
