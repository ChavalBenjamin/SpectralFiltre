#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SpectralCurveEngine.h"
#include "SpectralCurvePreviewControl.h"
#include "SpectralFilterEngine.h"
#include "BrickwallLimiter.h"
#include "SpectrumAnalyzer.h"
#include <atomic>
#include <mutex>

// ============================================================================
// SpectralFiltre - module mono-effet, issu du projet fusionne "Les
// Operateurs Spectraux" (desormais separe en 3 plugins distincts). Filtre
// spectral par bande (mapping log Hz 20-20kHz, +/-24dB), pilote par une
// courbe spectrale. Limiteur Brickwall en securite finale. Stereo.
// ============================================================================

enum EParams
{
  kParamFFTSize = 0,   // 0=512..4=8192
  kParamOverlap,       // 0=2x, 1=4x
  kParamCycles,
  kParamQ,
  kParamBallade,
  kParamHorizon,
  kParamSkew,
  kParamShapeMode,        // 0 = Type, 1 = Dessin libre
  kParamDryWet,           // 0-100% : melange signal sec (retarde, compense) / traite
  kParamLimiterThreshold, // dB - securite finale
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class SpectralFiltre final : public iplug::Plugin
{
public:
  SpectralFiltre(const InstanceInfo& info);

  void OnIdle() override;
  void OnUIOpen() override { SyncUIToState(); }
  void OnUIClose() override { mCurveView = nullptr; for (auto& c : mParamControls) c = nullptr; }

  // Dessin libre non sauvegarde entre sessions Reaper (SerializeState
  // retire par le passe suite a un crash grave - voir historique du
  // projet "Les Operateurs Spectraux").

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnParamChange(int paramIdx) override;
  void OnReset() override;
#endif

private:
  SpectralCurvePreviewControl* mCurveView = nullptr;
  IControl* mParamControls[kNumParams] = { nullptr };

  std::vector<float> mDrawnShapeStorage;

  void ApplyAllState();
  void SyncUIToState();

#if IPLUG_DSP
  void UpdateFFTConfig();
  void UpdateEngine();
  void UpdateYAxisMarks();

  SpectralCurveEngine mEngine;
  SpectralFilterEngine mFilterL, mFilterR;
  BrickwallLimiter mLimiter;
  SpectrumAnalyzer mAnalyzer;

  // Protege le moteur : Init() (thread principal, au changement de FFT
  // Size) ne doit jamais s'executer en meme temps que Process() (thread
  // audio) - lecon tiree d'un crash reproductible a grande taille FFT
  // sur le projet fusionne precedent.
  std::mutex mEngineMutex;

  // Ligne a retard du signal sec, alignee EXACTEMENT sur la latence du
  // traitement (une fenetre FFT), pour que Dry/Wet ne cree pas de
  // decalage temporel.
  std::mutex mDryDelayMutex;
  std::vector<float> mDryDelayL, mDryDelayR;
  int mDryDelayPos = 0;
  int mDryDelaySize = 1;

  std::mutex mCurveMutex;
  std::vector<float> mSharedCurve;

  std::mutex mSpectrumMutex;
  std::atomic<bool> mSpectrumUIUpdated { false };
  float mSpectrumUIBuf[1100] = { -80.f };
  int mSpectrumUISize = 0;

  std::atomic<bool> mCurveUIUpdated { false };
  float mCurveUIBuf[512] = { 0.f };
  int mCurveUISize = 0;
#endif
};
