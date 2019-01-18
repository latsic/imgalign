
import * as EXIF from 'exif-js'

export class ExifHelper
{
  static async getFieldOfViewAsync(img)
  {
    return new Promise(resolve => {

      EXIF.getData(img, function() {
        const fl = EXIF.getTag(this, 'FocalLengthIn35mmFilm');
        
        if(fl) {
          
          let flN = Number(fl);
          if(!isNaN(flN)) {
            
            const getAspectRatio = () => {
              let w = EXIF.getTag(this, 'ImageWidth');
              let h = EXIF.getTag(this, 'ImageHeight');

              const pW = EXIF.getTag(this, 'PixelXDimension');
              const pH = EXIF.getTag(this, 'PixelYDimension');
              if(pW) w = pW;
              if(pH) h = pH;
              
              if(!w || !h) return null;
              let wN = Number(w);
              if(isNaN(wN)) return null;
              let hN = Number(h);
              if(isNaN(hN)) return null;

              // const o = EXIF.getTag(this, 'Orientation');
              // if(o && (o == 6 || o == 8)) {
              //   return h / w;
              // }
              
              return w / h;
            };
            
            
            if(flN) {
              // Angle of view horizontally
              let va = 180 * 35 / (Math.PI * flN);
              const aspectRatio = getAspectRatio();
              if(aspectRatio && aspectRatio < 1) {
                va *= aspectRatio;
              }

              resolve(Math.round(va));
              return;
            }
          }
        }
        resolve(null);
      });
    });
  }
}
