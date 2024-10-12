# Anti-Derivative Antialiasing Waveshaping Distortion

This repo contains source code of a $\tanh$ distortion plugin which is antialiased using ADAA. 

## Transparent ADAA (M. Vicanek)

Next to the original ADAA method, I implemented Vicanek's version which does not have an impulse response with a 0dp pole at nyquist.

## Interpolation

The following modes are available:
* No interpolation
* Linear interpolation
* Natural cubic spline interpolation

When using linear interpolation of the antiderivative, this results in a bit crushing effect which is very noticeable at low gain.
This disappears for cubic spline interpolation.

## Roadmap

* Implement oversampling
* Other waveshaping types
* Better DC-Offset Filter
