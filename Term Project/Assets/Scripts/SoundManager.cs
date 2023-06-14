using UnityEngine;

public class SoundManager : MonoBehaviour
{
    public AudioClip soundEffect;
    public float repeatInterval = 5f;

    private AudioSource audioSource;

    private void Start()
    {
        audioSource = GetComponent<AudioSource>();
        PlaySoundEffectRepeatedly();
    }

    private void PlaySoundEffectRepeatedly()
    {
        audioSource.clip = soundEffect;
        audioSource.loop = true;
        audioSource.PlayDelayed(repeatInterval);
    }
}
