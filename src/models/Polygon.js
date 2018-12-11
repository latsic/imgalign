export class Polygon {
  
  constructor(pts = []) {
    this._pts = pts;
  }

  addPt(pt) {
    this._pts.push(pt);
  }

  addPtAndCloseIf(pt, maxDistToStart = 0) {
    if(this.pts.length < 3) {
      this.addPt(pt);
      return;
    }
    const { x, y } = this.pts[0];
    if(Math.pow(Math.abs(x - pt.x), 2)
     + Math.pow(Math.abs(y - pt.y), 2)
     < Math.pow(maxDistToStart, 2)) {

      this.addPt({ x, y });
    }
    else {
      this.addPt(pt);
    }
  }

  isClosed() {
    return this.pts.length > 3
      && this.pts[0].x == this.pts[this.pts.length - 1].x
      && this.pts[0].y == this.pts[this.pts.length - 1].y;
  }

  clear() {
    this._pts = [];
  }

  get pts() {
    return this._pts;
  }

  get bbox() {
    
    const tl = { x: Number.MAX_SAFE_INTEGER, y: Number.MAX_SAFE_INTEGER };
    const br = { x: Number.MIN_SAFE_INTEGER, y: Number.MIN_SAFE_INTEGER};
      
    for(let i = 0; i < this.pts.length; ++i) {
      
      const pt = this.pts[i];

      if(pt.x < tl.x) tl.x = pt.x;
      if(pt.x > br.x) br.x = pt.x;
      
      if(pt.y < tl.y) tl.y = pt.y;
      if(pt.y > br.y) br.y = pt.y;
    }

    return { tl, br };
  }

  scaledCopy(scale) {
    const p = new Polygon();
    for(const pt of this.pts) {
      p.addPt({ x: pt.x * scale, y: pt.y * scale });
    }
    return p;
  }
}