using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RandomManager : MonoBehaviour
{
    private List<int> numbersList; // List of numbers to choose from
    private WaitForSeconds waitTime; // Wait time between selections

    private int selectedNumber = 0;

    public GameObject explosionPrefab;
    // Start is called before the first frame update
    public GameObject fogPrefab1;
    public GameObject fogPrefab2;
    public AudioClip fogSound; 
    public Camera mainCamera;
    private AudioSource fogAudioSource;
    public AudioClip destructionSound; 
    private AudioSource destructAudioSource;
    public AudioClip moveSound; 
    private AudioSource moveAudioSource;
    public AudioClip chargeSound; 
    private AudioSource chargeAudioSource;
    public AudioClip dischargeSound; 
    private AudioSource dischargeAudioSource;
    void Start()
    {
        mainCamera = Camera.main;
        numbersList = new List<int>() { 1, 2, 3, 4, 5, 6 }; // Initialize the numbersList with the desired numbers
        waitTime = new WaitForSeconds(10f); // Set the wait time to 5 seconds
        StartCoroutine(RandomSelectionLoop()); // Start the coroutine
    }


    // Coroutine for the random selection loop
    private IEnumerator RandomSelectionLoop()
    {
        int counter = -2;
        while (ScoreManager.Instance.GetScore() > 0)
        {
            
            yield return waitTime; // Wait for the specified time

            int randomIndex = Random.Range(0, numbersList.Count); // Get a random index
            selectedNumber = numbersList[randomIndex]; // Get the selected number
            selectedNumber = counter;
            Debug.Log("Selected number: " + selectedNumber);
            if(selectedNumber > 0){
            SubtitleManager.Instance.EnablePanel();
            SubtitleManager.Instance.currentDisplayingText = selectedNumber - 1;
            if(selectedNumber == 1){
                timasteBender();
            } else if (selectedNumber == 2){
                astralWeakining();
            }  else if (selectedNumber == 3){
                cosmicBlast();
            }  else if (selectedNumber == 4){
                cosmicFog();
            }  else if (selectedNumber == 5){
                lightSpeedEffect();
            }  else if (selectedNumber == 6){
                astralCharge();
            }
            counter += 1;
            counter = counter % 6;
            } else {
            counter += 1;
            
            }
            
        }
        SubtitleManager.Instance.DisablePanel();
    }

    void timasteBender()
    {
        moveAudioSource = gameObject.AddComponent<AudioSource>(); 
        moveAudioSource.PlayOneShot(moveSound);
        SpawnManager.Instance.moveSpeed = SpawnManager.Instance.moveSpeed / 3;
        StartCoroutine(RestoreAsteroidSpeedsForTimasteBender(5f));
    }

    private IEnumerator RestoreAsteroidSpeedsForTimasteBender(float delay)
    {
        yield return new WaitForSeconds(delay);
        SpawnManager.Instance.moveSpeed = SpawnManager.Instance.moveSpeed * 3;
        SubtitleManager.Instance.DisablePanel();
    }

    void astralWeakining(){
        dischargeAudioSource = gameObject.AddComponent<AudioSource>(); 
        dischargeAudioSource.PlayOneShot(dischargeSound);
        CollisionManager.Instance.currentAstralMode = 1;
        StartCoroutine(RestoreAstralMode(5f));
        
    }

    void astralCharge(){
        chargeAudioSource = gameObject.AddComponent<AudioSource>(); 
        chargeAudioSource.PlayOneShot(chargeSound);
        CollisionManager.Instance.currentAstralMode = 2;
        StartCoroutine(RestoreAstralMode(5f));
    }

    private IEnumerator RestoreAstralMode(float delay)
    {
        yield return new WaitForSeconds(delay);
        CollisionManager.Instance.currentAstralMode = 0; 
        SubtitleManager.Instance.DisablePanel();   
    }

    void cosmicBlast(){
        foreach (GameObject asteroid in SpawnManager.Instance.attackersOnScreen)
        {
            if(asteroid != null){
                GameObject explosion = Instantiate(explosionPrefab, asteroid.transform.position, Quaternion.identity);
                explosion.transform.localScale = asteroid.transform.localScale;
                Destroy(asteroid);
            }
            destructAudioSource = gameObject.AddComponent<AudioSource>(); 
            destructAudioSource.PlayOneShot(destructionSound);
        }
        SpawnManager.Instance.attackersOnScreen.Clear();
        StartCoroutine(RestoreCosmicBlast(5f));
    }

    private IEnumerator RestoreCosmicBlast(float delay)
    {
        yield return new WaitForSeconds(delay);
        SubtitleManager.Instance.DisablePanel();   
    }

    void cosmicFog(){
        Vector3 cameraPosition = mainCamera.transform.position;
        GameObject fog1 = Instantiate(fogPrefab1, cameraPosition, Quaternion.identity);
        GameObject fog2 = Instantiate(fogPrefab2, cameraPosition, Quaternion.identity);
        GameObject fog3 = Instantiate(fogPrefab1, cameraPosition, Quaternion.identity);
        GameObject fog4 = Instantiate(fogPrefab2, cameraPosition, Quaternion.identity);
        fogAudioSource = gameObject.AddComponent<AudioSource>(); 
        fogAudioSource.clip = fogSound; 
        fogAudioSource.volume = 0.5f; // Set the volume to a lower value (e.g., 0.5) 
        fogAudioSource.Play();
        StartCoroutine(RestoreFog(5f, fog1, fog2, fog3, fog4));
    }
    private IEnumerator RestoreFog(float delay, GameObject fog1, GameObject fog2,GameObject fog3, GameObject fog4)
    {
        yield return new WaitForSeconds(delay);
        Destroy(fog1);
        Destroy(fog2);
        Destroy(fog3);
        Destroy(fog4);
        if(fogAudioSource != null){
            fogAudioSource.Stop();
        }
        SubtitleManager.Instance.DisablePanel();
    }
    void lightSpeedEffect(){
        moveAudioSource = gameObject.AddComponent<AudioSource>(); 
        moveAudioSource.PlayOneShot(moveSound);
        SpawnManager.Instance.moveSpeed = SpawnManager.Instance.moveSpeed * 3;
        StartCoroutine(RestoreAsteroidSpeedsForLightSpeedEffect(5f));
    }

    private IEnumerator RestoreAsteroidSpeedsForLightSpeedEffect(float delay)
    {
        yield return new WaitForSeconds(delay);
        SpawnManager.Instance.moveSpeed = SpawnManager.Instance.moveSpeed / 3;
        SubtitleManager.Instance.DisablePanel();
    }

    
}
