using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseManager : MonoBehaviour
{
    public Camera cameraObj;
    public GameObject myGameObj;
    public float speed = 0.5f;

    private Vector3 lastMousePosition;

    float zoom = 10f;
    
    private float lastClickTime;
    private float doubleClickTimeThreshold = 0.3f;

    public GameObject explosionPrefab;

    public AudioClip destructionSound; 
    private AudioSource destructAudioSource;
    void Update()
    {
        RotateCamera();
        myGameObj.transform.Rotate(0, 50 * Time.deltaTime, 0);
        ZoomCamera();
        DestroyClickedObject();
        
    }
    
    void DestroyClickedObject()
    {
        if (Input.GetMouseButtonDown(0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            GameObject objectHit;

            if (Physics.Raycast(ray, out hit, 100))
            {
                objectHit = hit.transform.gameObject;

                if (objectHit.name != "Earth")
                {
                    if (Time.time - lastClickTime < doubleClickTimeThreshold)
                    {
                        // Double-click event
                        Debug.Log("Double-clicked on: " + objectHit.name);
                        Vector3 objectSize = objectHit.transform.localScale;
                        GameObject explosion = Instantiate(explosionPrefab, objectHit.transform.position, Quaternion.identity);
                        explosion.transform.localScale = objectSize;
                        Destroy(objectHit);
                        SpawnManager.Instance.asteroids.Remove(objectHit);
                        destructAudioSource = gameObject.AddComponent<AudioSource>(); 
                        destructAudioSource.PlayOneShot(destructionSound);
                    }

                    lastClickTime = Time.time;
                }
            }
        }
    }

    void ZoomCamera(){
        if(cameraObj.orthographic){
            cameraObj.orthographicSize -= Input.GetAxis("Mouse ScrollWheel") * zoom;
        } else {
            cameraObj.fieldOfView -= Input.GetAxis("Mouse ScrollWheel") * zoom;
        }
    }

    void RotateCamera()
    {
        if (Input.GetMouseButtonDown(0))
        {
            lastMousePosition = Input.mousePosition;
        }

        if (Input.GetMouseButton(0))
        {
            Vector3 deltaMousePosition = Input.mousePosition - lastMousePosition;

            cameraObj.transform.RotateAround(myGameObj.transform.position,
                                             cameraObj.transform.up,
                                             -deltaMousePosition.x * speed);

            cameraObj.transform.RotateAround(myGameObj.transform.position,
                                             cameraObj.transform.right,
                                             -deltaMousePosition.y * speed);

            lastMousePosition = Input.mousePosition;
        }
    }
}
