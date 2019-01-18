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

  static imageSrcFromImageData(imageData) {

    const startTime = new Date();

    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    ctx.globalAlpha
    canvas.width = imageData.width;
    canvas.height = imageData.height;
    
    ctx.putImageData(imageData, 0, 0);
    let cData = ctx.getImageData(0, 0, imageData.width, imageData.height).data;

    for(let i = 3; i < imageData.length; i += 4) {
      cData[i] = 0;//imageData[i];
    }

    // const canvas2 = document.createElement('canvas');
    // canvas2.width = imageData.width;
    // canvas2.height = imageData.height;
    // const ctx2 = canvas2.getContext('2d');
    // ctx2.rect(0, 0, imageData.width, imageData.height);
    // ctx2.fillStyle = "white";
    // ctx2.fill();
    // ctx2.drawImage(canvas, 0, 0);

    const url = canvas.toDataURL();

    // eslint-disable-next-line no-console
    console.log('[imageSrcFromImageData][toDataURL][time][ms]', new Date() - startTime);

    return url;
  }
}