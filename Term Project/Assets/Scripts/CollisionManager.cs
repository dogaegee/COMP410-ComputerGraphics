using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CollisionManager : MonoBehaviour
{
    public static CollisionManager Instance { get; private set; }

    public GameObject explosionPrefab;

    public int ufoNormal = -40;
    public int astreoidNormal = -20;

    public int ufoWeakened = -20;
    public int astreoidWeakened = -10;

    public int ufoCharged = -80;
    public int astreoidCharged = -40;

    public int currentAstralMode = 0;

    public AudioClip destructionSound; 
    private AudioSource destructAudioSource;
    private void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
        }
        else
        {
            Instance = this;
            DontDestroyOnLoad(gameObject);
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.CompareTag("Astreoid") || collision.gameObject.CompareTag("UFO"))
        {
            GameObject explosion = Instantiate(explosionPrefab, collision.transform.position, Quaternion.identity);
            destructAudioSource = gameObject.AddComponent<AudioSource>(); 
            destructAudioSource.PlayOneShot(destructionSound);
            int life = ScoreManager.Instance.GetScore();
            if (life > 0)
            {
                int ufoDecrease = ufoNormal;;
                int astreoidDecrease = astreoidNormal;;
                if (currentAstralMode == 0)
                {
                    ufoDecrease = ufoNormal;
                    astreoidDecrease = astreoidNormal;
                }
                else if (currentAstralMode == 1)
                {
                    ufoDecrease = ufoWeakened;
                    astreoidDecrease = astreoidWeakened;
                }
                else if (currentAstralMode == 2)
                {
                    ufoDecrease = ufoCharged;
                    astreoidDecrease = astreoidCharged;
                }
                

                if (collision.gameObject.CompareTag("UFO"))
                {
                    ScoreManager.Instance.UpdateScore(ufoDecrease);
                }
                else
                {
                    ScoreManager.Instance.UpdateScore(astreoidDecrease);
                }
            }

            Destroy(collision.gameObject);

            SpawnManager.Instance.asteroids.Remove(collision.gameObject);
        }
    }
}
