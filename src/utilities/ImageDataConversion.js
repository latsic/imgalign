export class ImageDataConversion {
  
  static imageDataFromImageSrc(imageSrc) {

    const startTime = new Date();

    const canvas = document.createElement('canvas');
    canvas.width = imageSrc.width;
    canvas.height = imageSrc.height;
    const ctx = canvas.getContext('2d');
    ctx.drawImage(imageSrc, 0, 0, imageSrc.width, imageSrc.height);
    const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);

    // eslint-disable-next-line no-console
    console.log('[imageDataFromImageSrc][time][ms]', new Date() - startTime);

    return imageData;
  }

  static setImageSrcFromImageData(imageSrc, imageData) {
    
    const startTime = new Date();

    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    canvas.width = imageData.width;
    canvas.height = imageData.height;
    ctx.putImageData(imageData, 0, 0);
    imageSrc = canvas.toDataURL();

    // eslint-disable-next-line no-console
    console.log('[setImageSrcFromImageData][time][ms]', new Date() - startTime);
  }

  static imageSrcFromImageData(imageData) {

    const startTime = new Date();

    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    canvas.width = imageData.width;
    canvas.height = imageData.height;
    ctx.putImageData(imageData, 0, 0);
    const url = canvas.toDataURL();

    // eslint-disable-next-line no-console
    console.log('[imageSrcFromImageData][toDataURL][time][ms]', new Date() - startTime);

    return url;
  }
}